/**
 * @file socket.hpp
 * @brief Defines the TCPSocket class for handling TCP socket connections.
 */

#pragma once

#include "socket_address.hpp"

namespace hash_server {

/**
 * @class TCPSocket
 * @brief This class provides methods for handling TCP socket connections.
 */
class TCPSocket {
 public:
  /**
   * @brief Default constructor. Creates a new TCP socket.
   */
  TCPSocket();

  /**
   * @brief Constructor that takes a file descriptor.
   * @param fd The file descriptor of the socket.
   */
  explicit TCPSocket(int fd);

  /**
   * @brief Destructor. Closes the socket.
   */
  ~TCPSocket();

  /**
   * @brief Move constructor.
   * @param other The other TCPSocket object.
   */
  TCPSocket(TCPSocket &&other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The other TCPSocket object.
   * @return A reference to this object.
   */
  TCPSocket &operator=(TCPSocket &&other) noexcept;

  /**
   * @brief Deleted copy constructor.
   */
  TCPSocket(const TCPSocket &) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  TCPSocket &operator=(const TCPSocket &) = delete;

  /**
   * @brief Binds the socket to an address.
   * @param address The address to bind the socket to.
   */
  void Bind(const hash_server::SocketAddress &address) const;

  /**
   * @brief Listens for connections on the socket.
   * @param max_queue The maximum number of queued connections.
   */
  void Listen(int max_queue) const;

  /**
   * @brief Gets the file descriptor of the socket.
   * @return The file descriptor of the socket.
   */
  int GetFd() const;

  /**
   * @brief Reads data from the socket.
   * @param size The number of bytes to read.
   * @return The data read from the socket.
   */
  std::string Read(size_t size) const;

  /**
   * @brief Writes data to the socket.
   * @param data The data to write to the socket.
   * @return The number of bytes written.
   */
  size_t Write(const std::string &data) const;

  /**
   * @brief Accepts a new connection on the socket.
   * @return A new TCPSocket object for the accepted connection.
   */
  TCPSocket Accept() const;

  /**
   * @brief Connects the socket to an address.
   * @param address The address to connect the socket to.
   */
  void Connect(const hash_server::SocketAddress &address) const;

 private:
  /**
   * @brief The file descriptor of the socket.
   */
  int fd_ = -1;
};

}  // namespace hash_server