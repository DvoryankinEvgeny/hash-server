/**
 * @file epoll.hpp
 * @brief Defines the EPoll class for handling epoll operations.
 */

#pragma once

#include <sys/epoll.h>

#include <chrono>
#include <stdexcept>
#include <vector>

#include "socket.hpp"

namespace hash_server {

/**
 * @enum EPollDirection
 * @brief Enumeration for the direction of epoll operations.
 */
enum class EPollDirection {
  kReadFrom = EPOLLIN,              ///< Notify when socket is ready for reading.
  kWriteTo = EPOLLOUT,              ///< Notify when socket is ready for writing.
  kReadWrite = EPOLLIN | EPOLLOUT,  ///< Both read from and write to the socket.
};

/**
 * @class EPoll
 * @brief This class provides methods for handling epoll operations.
 */
class EPoll {
 public:
  /**
   * @brief Constructor that takes the maximum number of events. Creates a new epoll instance.
   * @param max_events The maximum number of events that epoll can handle at once.
   */
  EPoll(size_t max_events);

  /**
   * @brief Destructor. Closes the epoll instance.
   */
  ~EPoll();

  /**
   * @brief Deleted copy constructor.
   */
  EPoll(const EPoll&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  EPoll& operator=(const EPoll&) = delete;

  /**
   * @brief Deleted move constructor.
   */
  EPoll(EPoll&&) = delete;

  /**
   * @brief Deleted move assignment operator.
   */
  EPoll& operator=(EPoll&&) = delete;

  /**
   * @brief Adds a socket to the epoll instance.
   * @param socket The socket to add.
   * @param direction The direction of the epoll operation.
   */
  void Add(const TCPSocket& socket, EPollDirection direction);

  /**
   * @brief Deletes a socket from the epoll instance.
   * @param socket The socket to delete.
   */
  void Delete(const TCPSocket& socket);

  /**
   * @brief Modifies the epoll operation for a socket.
   * @param socket The socket to modify.
   * @param direction The new direction of the epoll operation.
   */
  void Modify(const TCPSocket& socket, EPollDirection direction);

  /**
   * @brief Waits for events on the epoll instance.
   * @param timeout The maximum time to wait for events.
   * @return A vector of file descriptors for the sockets with events.
   */
  std::vector<int> Wait(std::chrono::milliseconds timeout);

 private:
  /**
   * @brief The file descriptor of the epoll instance.
   */
  int fd_ = -1;
  /**
   * @brief The events to handle.
   */
  std::vector<struct epoll_event> events_;
};

}  // namespace hash_server