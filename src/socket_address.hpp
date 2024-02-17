/**
 * @file socket_address.hpp
 * @brief Defines the SocketAddress class for handling socket addresses.
 */

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstring>
#include <stdexcept>
#include <string>

namespace hash_server {

/**
 * @class SocketAddress
 * @brief This class provides methods for handling socket addresses.
 */
class SocketAddress {
 public:
  /**
   * @brief Constructor that takes an address and a port.
   * @param address The address.
   * @param port The port.
   */
  SocketAddress(const std::string& address, unsigned port);

  /**
   * @brief Move constructor.
   * @param other The other SocketAddress object.
   */
  SocketAddress(SocketAddress&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The other SocketAddress object.
   * @return A reference to this object.
   */
  SocketAddress& operator=(SocketAddress&& other) noexcept;

  /**
   * @brief Deleted copy constructor.
   */
  SocketAddress(const SocketAddress&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  SocketAddress& operator=(const SocketAddress&) = delete;

  /**
   * @brief Gets the plain socket address.
   * @return The plain socket address.
   */
  const struct sockaddr_in& GetPlainSocketAddress() const;

 private:
  /**
   * @brief The socket address.
   */
  struct sockaddr_in socket_address_;
};

}  // namespace hash_server