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

auto main(int argc, char* argv[]) -> int {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << "<address> <port>\n";
    std::cout << "Example: " << argv[0] << "0.0.0.0 12345\n";
    return 1;
  }
  try {
    hash_server::ServerConfiguration config{};
    config.thread_pool_size = std::thread::hardware_concurrency();
    config.epoll_max_events = kEPollMaxEvents;
    config.socket_read_buffer_size = kSocketReadBufferSize;
    config.select_max_queue_size = kSelectMaxQueueSize;
    config.hash_type = hash_server::HasherType::kSHA256;
    config.stop_symbol = '\n';
    config.polling_type = hash_server::PollingType::kEPoll;

    auto poller = hash_server::CreateSocketPoller(config.polling_type);

    std::cout << "Starting server on " << argv[1] << ":" << argv[2] << "\n";

    const std::string address(argv[1]);
    const unsigned port = std::stoul(argv[2]);

    hash_server::Server server(hash_server::SocketAddress{address, port}, std::move(config), std::move(poller));
    server.RunLoop();
  } catch (const std::exception &ex) {
    std::cout << "Something went wrong: " << ex.what() << "\n";
  } catch (...) {
    std::cout << "Something went very very wrong\n";
  }
  return 0;
}
