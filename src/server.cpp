#include "server.hpp"

#include <iostream>

namespace hash_server {

namespace {
std::vector<std::pair<std::string_view, bool>> SplitStringBySymbol(std::string_view str, char symbol) {
  std::vector<std::pair<std::string_view, bool>> result;
  size_t start = 0;
  while (start < str.size()) {
    const auto end = str.find(symbol, start);
    if (end == str.npos) {
      result.emplace_back(str.substr(start), false);
      break;
    }
    result.emplace_back(str.substr(start, end - start), true);
    start = end + 1;
  }
  return result;
}
}  // namespace

Server::Server(SocketAddress &&address, ServerConfiguration &&config)
    : config_(std::move(config)), address_(std::move(address)), thread_pool_(config_.thread_pool_size) {}

void Server::RunLoop() {
  socket_.Bind(address_);
  socket_.Listen(config_.select_max_queue_size);
  epoll_.Add(socket_, EPollDirection::kReadFrom);

  stopped_ = false;

  while (!stopped_) {
    const auto readyDescriptors = epoll_.Wait(std::chrono::milliseconds{3000}, config_.epoll_max_events);
    for (const auto descriptor : readyDescriptors) {
      if (descriptor == socket_.GetFd()) {
        AcceptNewClient();
      } else {
        HandleClientData(descriptor);
      }
    }
  }
}

void Server::HandleClientData(int descriptor) {
  // Get data associated with the socket
  auto &client_data = clients_sockets_.at(descriptor);

  // Unsubscribe this socket from any events until we process the data
  epoll_.Delete(client_data.socket_);

  // Create a task to process the data
  thread_pool_.AddTask([this, &client_data] {
    auto &[client_socket, hasher] = client_data;

    auto client_data = client_socket.Read(config_.socket_read_buffer_size);
    if (client_data.empty()) {
      // Socket is already closed, so just remove it. There is nothing to do.
      clients_sockets_.erase(client_socket.GetFd());
      return;
    }

    const auto strings = SplitStringBySymbol(client_data, '\n');
    for (const auto &[str, is_complete] : strings) {
      hasher.Update(str);
      if (is_complete) {
        std::string response = hasher.Finalize();
        client_socket.Write(response);
      }
    }

    // Subscribe this socket for reading again
    epoll_.Add(client_socket, EPollDirection::kReadFrom);
  });
}

void Server::Stop() { stopped_ = true; }

void Server::AcceptNewClient() {
  auto new_socket = socket_.Accept();

  epoll_.Add(new_socket, EPollDirection::kReadFrom);

  const auto fd = new_socket.GetFd();
  clients_sockets_.emplace(fd, std::move(new_socket));
}

}  // namespace hash_server