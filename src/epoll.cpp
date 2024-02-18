#include "epoll.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>

#include "errors_helper.hpp"
#include "socket.hpp"

namespace hash_server {

namespace {
constexpr size_t kEPollSize = 1;  // Parameter is obsolete and just should be grater than 0

int PollingDirectionToEpoll(PollingDirection direction) {
  switch (direction) {
    case PollingDirection::kReadFrom:
      return EPOLLIN;
    case PollingDirection::kWriteTo:
      return EPOLLOUT;
    case PollingDirection::kReadWrite:
    default:
      return EPOLLIN | EPOLLOUT;
  }
}
}  // namespace

class EPoll : public SocketPoller {
 public:
  EPoll();
  ~EPoll();

  void Add(const TCPSocket& socket, PollingDirection direction) override;
  void Delete(const TCPSocket& socket) override;
  void Modify(const TCPSocket& socket, PollingDirection direction) override;
  std::vector<int> Wait(std::chrono::milliseconds timeout, size_t max_events) override;

 private:
  int fd_ = -1;
};

EPoll::EPoll() : fd_(epoll_create(kEPollSize)) {
  if (fd_ < 0) {
    ThrowRuntimeError("Failed to create a new epoll instance");
  }
}

EPoll::~EPoll() {
  if (fd_ > 0) close(fd_);
}

void EPoll::Add(const TCPSocket& socket, PollingDirection direction) {
  struct epoll_event event {};

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = PollingDirectionToEpoll(direction);
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_ADD, socket.GetFd(), &event);
  if (error < 0) {
    ThrowRuntimeError("Failed to add socket to epoll instance");
  }
}

void EPoll::Delete(const TCPSocket& socket) {
  if (epoll_ctl(fd_, EPOLL_CTL_DEL, socket.GetFd(), nullptr) < 0) {
    ThrowRuntimeError("Failed to delete socket from epoll instance");
  }
}

void EPoll::Modify(const TCPSocket& socket, PollingDirection direction) {
  struct epoll_event event {};

  memset(&event, 0, sizeof(struct epoll_event));

  event.events = PollingDirectionToEpoll(direction);
  event.data.fd = socket.GetFd();
  int error = epoll_ctl(fd_, EPOLL_CTL_MOD, socket.GetFd(), &event);
  if (error < 0) {
    ThrowRuntimeError("Failed to modify socket in epoll instance");
  }
}

std::vector<int> EPoll::Wait(std::chrono::milliseconds timeout, size_t max_events) {
  std::vector<struct epoll_event> events(max_events);
  int epoll_ret = epoll_wait(fd_, events.data(), static_cast<int>(events.size()), static_cast<int>(timeout.count()));
  if (epoll_ret < 0) {
    ThrowRuntimeError("Failed to wait for events on epoll instance");
  }

  std::vector<int> ready_sockets;
  ready_sockets.reserve(epoll_ret);
  for (int i = 0; i < epoll_ret; ++i) {
    if (events[i].events & EPOLLERR) {
      // Let's not return sockets with errors
      continue;
    }
    ready_sockets.push_back(events[i].data.fd);
  }
  ready_sockets.shrink_to_fit();
  return ready_sockets;
}

std::unique_ptr<SocketPoller> CreateSocketPoller(PollingType) { return std::make_unique<EPoll>(); }

}  // namespace hash_server