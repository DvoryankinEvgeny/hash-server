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

EPoll::EPoll() {
  fd_ = epoll_create(255);
  if (fd_ < 0) {
    throw std::invalid_argument("ERROR epoll_create");
  }
}

EPoll::~EPoll() {
  if (fd_ > 0) close(fd_);
}

void EPoll::Add(const TCPSocket& socket, int events) {
  struct epoll_event event;

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = events;
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_ADD, socket.GetFd(), &event);
  if (error < 0) {
    std::string message("ERROR on EPOLL_CTL_ADD ");
    throw std::invalid_argument(message + strerror(errno));
  }
}

void EPoll::Delete(const TCPSocket& socket) {
  if (epoll_ctl(fd_, EPOLL_CTL_DEL, socket.GetFd(), nullptr) < 0) {
    throw std::invalid_argument("ERROR on EPOLL_CTL_DEL");
  }
}

void EPoll::Modify(const TCPSocket& socket, int events) {
  struct epoll_event event;

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = events;
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_MOD, socket.GetFd(), &event);
  if (error < 0) {
    std::string message("ERROR on EPOLL_CTL_MOD ");
    throw std::invalid_argument(message + strerror(errno));
  }
}

std::vector<int> EPoll::Wait(std::chrono::milliseconds timeout) {
  int epoll_ret = epoll_wait(fd_, events.data(), kMaxEvents, timeout.count());
  if (epoll_ret == -1) {
    throw std::invalid_argument("ERROR on epoll_wait");
  }

  std::vector<int> ready_sockets;
  ready_sockets.reserve(epoll_ret);
  for (int i = 0; i < epoll_ret; ++i) {
    if (events[i].events & EPOLLERR) {
      std::cout << "Epoll failed for socket: " << events[i].data.fd << "\n";
      continue;
    }
    ready_sockets.push_back(events[i].data.fd);
  }
  return ready_sockets;
}

}  // namespace hash_server