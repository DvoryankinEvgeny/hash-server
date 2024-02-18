#include "socket_address.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstring>

#include "errors_helper.hpp"

namespace hash_server {

SocketAddress::SocketAddress(const std::string& address, unsigned port) {
  memset(&socket_address_, 0, sizeof(socket_address_));
  if (inet_pton(AF_INET, address.c_str(), &(socket_address_.sin_addr)) != 1) ThrowRuntimeError("Wrong socket address");

  socket_address_.sin_port = htons(port);
  socket_address_.sin_family = AF_INET;
}

SocketAddress::SocketAddress(SocketAddress&& other) noexcept { std::swap(socket_address_, other.socket_address_); }

SocketAddress& SocketAddress::operator=(SocketAddress&& other) noexcept {
  std::swap(socket_address_, other.socket_address_);
  return *this;
}

const struct sockaddr_in& SocketAddress::GetPlainSocketAddress() const { return socket_address_; }

}  // namespace hash_server
