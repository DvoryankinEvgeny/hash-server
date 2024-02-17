#include "epoll.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "socket.hpp"

namespace hash_server {

namespace {
constexpr size_t kEPollSize = 255;  // Parameter is obsolete and just should be grater than 0
}  // namespace

EPoll::EPoll(size_t max_events) : fd_(epoll_create(kEPollSize)) {
  if (fd_ < 0) {
    throw std::invalid_argument("ERROR epoll_create");
  }
  events_.resize(max_events);
}

EPoll::~EPoll() {
  if (fd_ > 0) close(fd_);
}

void EPoll::Add(const TCPSocket& socket, EPollDirection direction) {
  struct epoll_event event {};

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = static_cast<int>(direction);
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_ADD, socket.GetFd(), &event);
  if (error < 0) {
    throw std::invalid_argument(std::string("ERROR on EPOLL_CTL_ADD ") + strerror(errno));
  }
}

void EPoll::Delete(const TCPSocket& socket) {
  if (epoll_ctl(fd_, EPOLL_CTL_DEL, socket.GetFd(), nullptr) < 0) {
    throw std::invalid_argument(std::string("ERROR on EPOLL_CTL_DEL ") + strerror(errno));
  }
}

void EPoll::Modify(const TCPSocket& socket, EPollDirection direction) {
  struct epoll_event event {};

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = static_cast<int>(direction);
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_MOD, socket.GetFd(), &event);
  if (error < 0) {
    throw std::invalid_argument(std::string("ERROR on EPOLL_CTL_MOD ") + strerror(errno));
  }
}

std::vector<int> EPoll::Wait(std::chrono::milliseconds timeout) {
  int epoll_ret = epoll_wait(fd_, events_.data(), events_.size(), timeout.count());
  if (epoll_ret < 0) {
    throw std::invalid_argument(std::string("ERROR on epoll_wait ") + strerror(errno));
  }

  std::vector<int> ready_sockets;
  ready_sockets.reserve(epoll_ret);
  for (int i = 0; i < epoll_ret; ++i) {
    if (events_[i].events & EPOLLERR) {
      std::cout << "Epoll failed for socket: " << events_[i].data.fd << "\n";
      continue;
    }
    ready_sockets.push_back(events_[i].data.fd);
  }
  return ready_sockets;
}

}  // namespace hash_server