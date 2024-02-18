#include <gtest/gtest.h>

std::string server_address;
unsigned server_port;

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
    std::cerr << "Example: " << argv[0] << " 0.0.0.0 12345\n";
    return 1;
  }

  server_address.assign(argv[1]);
  server_port = std::stoul(argv[2]);
  argc -= 2;  // google test doesn't need these two arguments
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}