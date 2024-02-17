#pragma once

#include <sys/epoll.h>

#include <array>
#include <chrono>
#include <stdexcept>
#include <vector>

#include "socket.hpp"

namespace hash_server {

enum class EPollDirection {
  kReadFrom = EPOLLIN,
  kWriteTo = EPOLLOUT,
  kReadWrite = EPOLLIN | EPOLLOUT,
};

class EPoll {
 public:
  EPoll();
  ~EPoll();

  EPoll(const EPoll&) = delete;
  EPoll& operator=(const EPoll&) = delete;
  EPoll(EPoll&&) = delete;
  EPoll& operator=(EPoll&&) = delete;

  void Add(const TCPSocket& socket, EPollDirection direction);
  void Delete(const TCPSocket& socket);
  void Modify(const TCPSocket& socket, EPollDirection direction);
  std::vector<int> Wait(std::chrono::milliseconds timeout);

 private:
  int fd_ = -1;
  static constexpr size_t kMaxEvents = 32;
  std::array<struct epoll_event, kMaxEvents> events;
};
}  // namespace hash_server