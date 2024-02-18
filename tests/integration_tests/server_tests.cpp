#include <gtest/gtest.h>

#include <condition_variable>
#include <iostream>
#include <memory>
#include <thread>

#include "polling.hpp"
#include "precomputed_hashes.hpp"
#include "server.hpp"
#include "socket.hpp"

extern std::string server_address;
extern unsigned server_port;

namespace {
const std::chrono::milliseconds kPollingTimeout{3000};
const size_t kExpectedResponseSize = 64;
const size_t kClientEpollMaxEvents = 5;
}  // namespace

class Client {
 public:
  Client() : poller_(hash_server::CreateSocketPoller(hash_server::PollingType::kEPoll)) {
    socket_.Connect(hash_server::SocketAddress(server_address, server_port));
    poller_->Add(socket_, hash_server::PollingDirection::kWriteTo);
  }

  void WaitForConnection() {
    const auto descriptors = poller_->Wait(kPollingTimeout, kClientEpollMaxEvents);
    ASSERT_EQ(descriptors.size(), 1);
    ASSERT_EQ(descriptors[0], socket_.GetFd());
  }

  std::string GetHashForString(const std::string& str) {
    socket_.Write(str + '\n');
    std::string response = ReadResponse();
    return response;
  }

  void SendString(const std::string& str) { socket_.Write(str); }

  std::string ReadResponse() {
    poller_->Modify(socket_, hash_server::PollingDirection::kReadFrom);
    const auto desc = poller_->Wait(kPollingTimeout, kClientEpollMaxEvents);
    std::string response;
    response.reserve(kExpectedResponseSize);
    while (response.size() < kExpectedResponseSize) {
      response += socket_.Read(kExpectedResponseSize);
    }
    socket_.Read(1);  // Skip stop-symbol
    return response;
  }

 private:
  hash_server::TCPSocket socket_;
  std::unique_ptr<hash_server::SocketPoller> poller_;
};

TEST(ServerFixture, SimpleHelloWorldTest) {
  Client client;
  client.WaitForConnection();
  std::string response = client.GetHashForString("Hello, World!");
  ASSERT_EQ(response, kPreComputedHashHelloWorld);
}

TEST(ServerFixture, MultipleClientsTest) {
  std::vector<std::thread> threads;
  const int kNumThreads = 50;
  threads.reserve(kNumThreads);
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back([i]() {
      Client client;
      client.WaitForConnection();
      std::string payload = i % 2 == 0 ? "Hello, World!" : "!dlroW ,olleH";
      std::string expected = i % 2 == 0 ? kPreComputedHashHelloWorld : kPreComputedHashHelloWorldReversed;
      std::string response = client.GetHashForString(payload);
      ASSERT_EQ(response, expected);
    });
  }

  for (auto& t : threads) {
    t.join();
  }
}

TEST(ServerFixture, DealingWithTheRestOfStringTest) {
  Client client;
  client.WaitForConnection();
  client.SendString("Hello, World!\n!dlroW ");
  auto response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorld);
  client.SendString(",olleH\n");
  response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorldReversed);
}

TEST(ServerFixture, MultipleStringInOneMessageTest) {
  Client client;
  client.WaitForConnection();
  client.SendString("\nHello, World!\n!dlroW ,olleH\n");
  auto response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashEmptyString);
  response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorld);
  response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorldReversed);
}
