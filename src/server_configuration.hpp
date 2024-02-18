/**
 * @file server_configuration.hpp
 * @brief Defines the ServerConfiguration struct for holding server configuration parameters.
 */

#pragma once

#include <thread>

#include "hasher.hpp"

namespace hash_server {

/**
 * @struct ServerConfiguration
 * @brief This struct holds the configuration parameters for the server.
 */
struct ServerConfiguration {
  /**
   * @brief The number of threads in the thread pool.
   */
  size_t thread_pool_size = 4;

  /**
   * @brief The maximum number of events that epoll can handle at once.
   */
  size_t epoll_max_events = 32;

  /**
   * @brief The size of the read buffer for the socket in bytes.
   */
  size_t socket_read_buffer_size = 1024;

  /**
   * @brief The maximum size of the queue for select.
   */
  size_t select_max_queue_size = 100;

  /**
   * @brief The type of the hasher.
   */
  HasherType hash_type = HasherType::kSHA256;

  /**
   * @brief The symbol that indicates the end of a message.
   */
  char stop_symbol = '\n';
};

}  // namespace hash_server