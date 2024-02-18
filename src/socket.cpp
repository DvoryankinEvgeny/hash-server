#include "socket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "errors_helper.hpp"
#include "socket_address.hpp"

namespace hash_server {

TCPSocket::TCPSocket() : fd_(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)) {
  if (fd_ < 0) ThrowRuntimeError("Failed to create a new socket");
  int optval = 1;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

TCPSocket::TCPSocket(int fd) : fd_(fd) {}

TCPSocket::~TCPSocket() {
  if (fd_ > 0) close(fd_);
}

TCPSocket::TCPSocket(TCPSocket &&other) noexcept { std::swap(fd_, other.fd_); }

TCPSocket &TCPSocket::operator=(TCPSocket &&other) noexcept {
  std::swap(fd_, other.fd_);
  return *this;
}

void TCPSocket::Bind(const hash_server::SocketAddress &address) const {
  const auto &plain_address = address.GetPlainSocketAddress();
  if (bind(fd_, (struct sockaddr *)&plain_address, sizeof(plain_address)) < 0)
    ThrowRuntimeError("Binding to address failed");
}

void TCPSocket::Listen(int max_queue) const {
  if (listen(fd_, max_queue) < 0) ThrowRuntimeError("Listen on socket failed");
}

int TCPSocket::GetFd() const { return fd_; }

std::string TCPSocket::Read(size_t size) const {
  std::string buffer(size, 0);
  ssize_t n = read(fd_, buffer.data(), size);
  if (n < 0) ThrowRuntimeError("Reading from the socket failed");
  buffer.resize(n);
  return buffer;
}

size_t TCPSocket::Write(std::string_view data) const {
  ssize_t n = write(fd_, data.data(), data.size());
  if (n < 0) ThrowRuntimeError("Writing to the socket failed");
  return n;
}

TCPSocket TCPSocket::Accept() const {
  int new_fd = accept(fd_, nullptr, nullptr);
  if (new_fd < 0) ThrowRuntimeError("Failed to accept a new connection");
  return TCPSocket(new_fd);
}

void TCPSocket::Connect(const hash_server::SocketAddress &address) const {
  const auto &plain_address = address.GetPlainSocketAddress();
  int error = connect(fd_, (struct sockaddr *)&plain_address, sizeof(plain_address));
  if (error < 0 && errno != EINPROGRESS) ThrowRuntimeError("Failed to connect to the server");
}
}  // namespace hash_server