#pragma once

#include <memory>
#include <unordered_map>

#include "hasher.hpp"
#include "polling.hpp"
#include "server_configuration.hpp"
#include "socket.hpp"
#include "socket_address.hpp"
#include "thread_pool.hpp"

namespace hash_server {

struct ClientData {
  TCPSocket socket_;
  std::unique_ptr<Hasher> data_;

  ClientData(TCPSocket&& socket, std::unique_ptr<Hasher>&& hasher)
      : socket_(std::move(socket)), data_(std::move(hasher)) {}
  ClientData() = delete;
};

class Server {
 public:
  explicit Server(SocketAddress&& address, ServerConfiguration&& config, std::unique_ptr<SocketPoller>&& poller);
  ~Server() = default;

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;

  void RunLoop();
  void Stop();

 private:
  void AcceptNewClient();
  void HandleClientData(int descriptor);

 private:
  bool stopped_ = true;
  ServerConfiguration config_;
  SocketAddress address_;
  TCPSocket socket_;
  std::unordered_map<int, ClientData> clients_sockets_;
  std::unique_ptr<SocketPoller> poller_;
  ThreadPool thread_pool_;
};
}  // namespace hash_server
