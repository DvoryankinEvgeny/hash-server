/**
 * @file polling.hpp
 * @brief Defines the SocketPoller class and related types for socket polling.
 */

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "socket.hpp"

namespace hash_server {

/**
 * @enum PollingDirection
 * @brief Enumeration for the direction of polling operations.
 */
enum class PollingDirection {
  kReadFrom,   ///< Read from the socket.
  kWriteTo,    ///< Write to the socket.
  kReadWrite,  ///< Both read from and write to the socket.
};

/**
 * @enum PollingType
 * @brief Enumeration for the types of polling.
 */
enum class PollingType {
  kEPoll,  ///< EPoll polling.
};

/**
 * @class SocketPoller
 * @brief This class provides an interface for socket polling.
 */
class SocketPoller {
 public:
  /**
   * @brief Default constructor.
   */
  SocketPoller() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~SocketPoller() = default;

  // Deleted copy and move constructors and assignment operators.
  SocketPoller(const SocketPoller&) = delete;
  SocketPoller& operator=(const SocketPoller&) = delete;
  SocketPoller(SocketPoller&&) = delete;
  SocketPoller& operator=(SocketPoller&&) = delete;

  /**
   * @brief Adds a socket to the poller.
   * @param socket The socket to add.
   * @param direction The direction of the polling operation.
   */
  virtual void Add(const TCPSocket& socket, PollingDirection direction) = 0;

  /**
   * @brief Deletes a socket from the poller.
   * @param socket The socket to delete.
   */
  virtual void Delete(const TCPSocket& socket) = 0;

  /**
   * @brief Modifies the polling operation for a socket.
   * @param socket The socket to modify.
   * @param direction The new direction of the polling operation.
   */
  virtual void Modify(const TCPSocket& socket, PollingDirection direction) = 0;

  /**
   * @brief Waits for events on the poller.
   * @param timeout The maximum time to wait for events.
   * @param max_events The maximum number of events to handle at once.
   * @return A vector of file descriptors for the sockets with events.
   */
  virtual std::vector<int> Wait(std::chrono::milliseconds timeout, size_t max_events) = 0;
};

/**
 * @brief Creates a socket poller of the specified type.
 * @param type The type of the poller.
 * @return A unique_ptr to the socket poller.
 */
std::unique_ptr<SocketPoller> CreateSocketPoller(PollingType type);

}  // namespace hash_server