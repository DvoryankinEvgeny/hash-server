#include "server.hpp"

#include <iostream>

namespace hash_server {

Server::Server(SocketAddress &&address) : address_(std::move(address)) {}

void Server::RunLoop() {
  std::cout << "Server::RunLoop\n";
  const int kMaxQueueSize = 10;
  socket_.Bind(address_);
  socket_.Listen(kMaxQueueSize);
  epoll_.Add(socket_, EPOLLIN | EPOLLPRI);

  stopped_ = false;

  while (!stopped_) {
    std::cout << "Server::Loop\n";
    const auto readyDescriptors = epoll_.Wait(std::chrono::milliseconds{3000});
    for (const auto descriptor : readyDescriptors) {
      std::cout << "Descriptor: " << descriptor << "\n";
      if (descriptor == socket_.GetFd()) {
        AcceptNewClient();
      } else {
        auto &client_data = clients_sockets_.at(descriptor);
        epoll_.Delete(client_data.socket_);
        thread_pool_.AddTask([this, &client_data] {
          const auto &soc = client_data.socket_;
          auto data = soc.Read(20);
          if (data.empty()) {
            std::cout << "EOF\n";
            clients_sockets_.erase(client_data.socket_.GetFd());
            return;
          }
          const auto pos = data.find('\n');
          const auto used_for_update = data.substr(0, pos);
          std::cout << "Line before\n";
          std::cout << "Used for update: " << used_for_update << "\n";
          std::cout << " size " << used_for_update.length() << "\n";
          client_data.data_.Update(used_for_update);
          if (pos != data.npos) {
          std::string response = client_data.data_.Finalize();
          client_data.socket_.Write(response);
          }

          if (pos != data.npos && pos != data.size() - 1) {
            const auto rest = data.substr(pos + 1);
            std::cout << "Rest: " << rest << " size " << rest.length() << "\n";
            client_data.data_.Update(rest);
          }
          epoll_.Add(client_data.socket_, EPOLLIN | EPOLLPRI);
        });
      }
    }
  }
}

void Server::Stop() {
  std::cout << "Server::Stop\n";
  stopped_ = true;
}

void Server::AcceptNewClient() {
  std::cout << "New connection\n";
  auto new_socket = socket_.Accept();
  if (new_socket.GetFd() < 0) throw std::runtime_error("ERROR on accept\n");

  epoll_.Add(new_socket, EPOLLIN | EPOLLPRI);
  const auto fd = new_socket.GetFd();
  clients_sockets_.emplace(fd, std::move(new_socket));
}

}  // namespace hash_server