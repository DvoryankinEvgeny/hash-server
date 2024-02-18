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

Server::Server(SocketAddress &&address, ServerConfiguration &&config, std::unique_ptr<SocketPoller> &&poller)
    : config_(std::move(config)),
      address_(std::move(address)),
      poller_(std::move(poller)),
      thread_pool_(config_.thread_pool_size) {}

void Server::RunLoop() {
  socket_.Bind(address_);
  socket_.Listen(config_.select_max_queue_size);
  poller_->Add(socket_, PollingDirection::kReadFrom);

  stopped_ = false;

  while (!stopped_) {
    const auto readyDescriptors = poller_->Wait(std::chrono::milliseconds{3000}, config_.epoll_max_events);
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
  poller_->Delete(client_data.socket_);

  // Create a task to process the data
  thread_pool_.AddTask([this, &client_data] {
    auto &[client_socket, hasher] = client_data;

    auto client_data = client_socket.Read(config_.socket_read_buffer_size);
    if (client_data.empty()) {
      // Socket is already closed, so just remove it. There is nothing to do.
      clients_sockets_.erase(client_socket.GetFd());
      return;
    }

    const auto strings = SplitStringBySymbol(client_data, config_.stop_symbol);
    for (const auto &[str, is_complete] : strings) {
      hasher->Update(str);
      if (is_complete) {
        std::string response = hasher->Finalize();
        client_socket.Write(response);
      }
    }

    // Subscribe this socket for reading again
    poller_->Add(client_socket, PollingDirection::kReadFrom);
  });
}

void Server::Stop() { stopped_ = true; }

void Server::AcceptNewClient() {
  auto new_socket = socket_.Accept();

  poller_->Add(new_socket, PollingDirection::kReadFrom);

  const auto fd = new_socket.GetFd();
  clients_sockets_.emplace(fd, ClientData{std::move(new_socket), CreateHasher(config_.hash_type)});
}

}  // namespace hash_server