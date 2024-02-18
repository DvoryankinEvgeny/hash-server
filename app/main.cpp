#include <threads.h>

#include <iostream>

#include "server.hpp"
#include "server_configuration.hpp"
#include "socket_address.hpp"

namespace {
const size_t kEPollMaxEvents = 100;
const size_t kSocketReadBufferSize = 1024 * 1024;
const size_t kSelectMaxQueueSize = 100;
}  // namespace

auto main() -> int {
  try {
    hash_server::ServerConfiguration config{};
    config.thread_pool_size = std::thread::hardware_concurrency();
    config.epoll_max_events = kEPollMaxEvents;
    config.socket_read_buffer_size = kSocketReadBufferSize;
    config.select_max_queue_size = kSelectMaxQueueSize;
    config.hash_type = hash_server::HasherType::kSHA256;
    const int portno = 12345;
    hash_server::Server server(hash_server::SocketAddress{"0.0.0.0", portno}, std::move(config));
    server.RunLoop();
  } catch (const std::exception &ex) {
    std::cout << "Something went wrong: " << ex.what() << "\n";
  } catch (...) {
    std::cout << "Something went very very wrong\n";
  }
  return 0;
}
