#include "gtest/gtest.h"
#include "socket_address.hpp"

namespace hash_server {

// Tests that the SocketAddress::SocketAddress constructor works as expected.
TEST(SocketAddressTest, Constructor) {
  SocketAddress addr("127.0.0.1", 8080);
  const struct sockaddr_in& plain_address = addr.GetPlainSocketAddress();
  EXPECT_EQ(plain_address.sin_port, htons(8080));
  EXPECT_EQ(plain_address.sin_addr.s_addr, inet_addr("127.0.0.1"));
}

// Tests that the SocketAddress::GetPlainSocketAddress method works as expected.
TEST(SocketAddressTest, GetPlainSocketAddress) {
  SocketAddress addr("127.0.0.1", 8080);
  const struct sockaddr_in& plain_address = addr.GetPlainSocketAddress();
  EXPECT_EQ(plain_address.sin_port, htons(8080));
  EXPECT_EQ(plain_address.sin_addr.s_addr, inet_addr("127.0.0.1"));
}

// Tests that the SocketAddress::SocketAddress move constructor works as expected.
TEST(SocketAddressTest, MoveConstructor) {
  SocketAddress addr1("127.0.0.1", 8080);
  SocketAddress addr2(std::move(addr1));

  const struct sockaddr_in& plain_address = addr2.GetPlainSocketAddress();
  EXPECT_EQ(plain_address.sin_port, htons(8080));
  EXPECT_EQ(plain_address.sin_addr.s_addr, inet_addr("127.0.0.1"));
}

}  // namespace hash_server