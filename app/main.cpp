#include <iostream>

#include "server.hpp"
#include "socket_address.hpp"

auto main() -> int {
  try {
    const int portno = 12345;
    hash_server::Server server(hash_server::SocketAddress{"0.0.0.0", portno});
    server.RunLoop();
  } catch (const std::exception &ex) {
    std::cout << "Something went wrong: " << ex.what() << "\n";
  } catch (...) {
    std::cout << "Something went very very wrong\n";
  }
  return 0;
}
