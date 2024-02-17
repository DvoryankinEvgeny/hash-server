/**
 * @file server_configuration.hpp
 * @brief Defines the ServerConfiguration struct for holding server configuration parameters.
 */

#pragma once

#include <thread>

namespace hash_server {

/**
 * @struct ServerConfiguration
 * @brief This struct holds the configuration parameters for the server.
 */
struct ServerConfiguration {
  /**
   * @brief The number of threads in the thread pool.
   */
  size_t thread_pool_size;

  /**
   * @brief The maximum number of events that epoll can handle at once.
   */
  size_t epoll_max_events;

  /**
   * @brief The size of the read buffer for the socket in bytes.
   */
  size_t socket_read_buffer_size;

  /**
   * @brief The maximum size of the queue for select.
   */
  size_t select_max_queue_size;
};

}  // namespace hash_server