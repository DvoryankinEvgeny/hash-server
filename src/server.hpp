#pragma once

#include <unordered_map>

#include "epoll.hpp"
#include "hasher.hpp"
#include "socket.hpp"
#include "socket_address.hpp"
#include "thread_pool.hpp"

namespace hash_server {

struct ClientData {
  TCPSocket socket_;
  Hasher data_;

  ClientData(TCPSocket&& socket) : socket_(std::move(socket)) {}
  ClientData() = delete;
};

class Server {
 public:
  explicit Server(SocketAddress&& address);
  ~Server() = default;

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;

  void RunLoop();
  void Stop();

 private:
  void AcceptNewClient();

 private:
  bool stopped_ = true;
  SocketAddress address_;
  TCPSocket socket_;
  std::unordered_map<int, ClientData> clients_sockets_;
  EPoll epoll_;
  ThreadPool thread_pool_{4};
};
}  // namespace hash_server
