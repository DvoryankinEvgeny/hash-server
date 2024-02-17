#include "server.hpp"

#include <iostream>

namespace hash_server {

Server::Server(SocketAddress &&address, ServerConfiguration &&config)
    : config_(std::move(config)), address_(std::move(address)), thread_pool_(config_.thread_pool_size) {}

void Server::RunLoop() {
  std::cout << "Server::RunLoop\n";
  socket_.Bind(address_);
  socket_.Listen(config_.select_max_queue_size);
  epoll_.Add(socket_, EPollDirection::kReadFrom);

  stopped_ = false;

  while (!stopped_) {
    std::cout << "Server::Loop\n";
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
    const auto pos = client_data.find('\n');
    const std::string_view used_for_update(client_data.data(), pos == client_data.npos ? client_data.size() : pos);
    hasher.Update(used_for_update);
    // If we have a complete line, calculate a hash it and send the response
    if (pos != client_data.npos) {
      std::string response = hasher.Finalize();
      client_socket.Write(response);
    }

    // If we have some data left, don't forget to use it
    if (pos != client_data.npos && pos < client_data.size() - 1) {
      const std::string_view rest(client_data.data() + pos + 2);
      hasher.Update(rest);
    }

    // Subscribe this socket for reading again
    epoll_.Add(client_socket, EPollDirection::kReadFrom);
  });
}

void Server::Stop() {
  std::cout << "Server::Stop\n";
  stopped_ = true;
}

void Server::AcceptNewClient() {
  std::cout << "New connection\n";
  auto new_socket = socket_.Accept();
  if (new_socket.GetFd() < 0) throw std::runtime_error("ERROR on accept\n");

  epoll_.Add(new_socket, EPollDirection::kReadFrom);
  const auto fd = new_socket.GetFd();
  clients_sockets_.emplace(fd, std::move(new_socket));
}

}  // namespace hash_server