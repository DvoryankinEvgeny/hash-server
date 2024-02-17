#include <gtest/gtest.h>

#include <condition_variable>
#include <iostream>
#include <memory>
#include <thread>

#include "epoll.hpp"
#include "precomputed_hashes.hpp"
#include "server.hpp"
#include "socket.hpp"

namespace {
const std::string kServerAddress("127.0.0.1");
const unsigned kServerPort = 2345;
const std::chrono::milliseconds kPollingTimeout{3000};
const size_t kExpectedResponseSize = 64;
const size_t kClientEpollMaxEvents = 5;
}  // namespace

class ServerFixture : public testing::Test {
 protected:
  static void SetUpTestSuite() {
    hash_server::ServerConfiguration config;
    config.thread_pool_size = 1;
    config.epoll_max_events = 10;
    config.socket_read_buffer_size = 2;
    config.select_max_queue_size = 10;
    server_ = std::make_unique<hash_server::Server>(hash_server::SocketAddress{kServerAddress, kServerPort},
                                                    std::move(config));
    server_thread_ = std::make_unique<std::thread>([]() {
      cv_.notify_all();
      server_->RunLoop();
    });

    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock);
  }

  static void TearDownTestSuite() {
    server_->Stop();
    if (server_thread_->joinable()) server_thread_->join();
    server_.reset(nullptr);
    server_thread_.reset(nullptr);
  }

  static std::unique_ptr<hash_server::Server> server_;
  static std::unique_ptr<std::thread> server_thread_;
  static std::condition_variable cv_;
  static std::mutex mutex_;
};

std::unique_ptr<hash_server::Server> ServerFixture::server_ = nullptr;
std::unique_ptr<std::thread> ServerFixture::server_thread_ = nullptr;
std::condition_variable ServerFixture::cv_;
std::mutex ServerFixture::mutex_;

class Client {
 public:
  Client() {
    socket_.Connect(hash_server::SocketAddress(kServerAddress, kServerPort));
    epoll_.Add(socket_, hash_server::EPollDirection::kWriteTo);
  }

  void WaitForConnection() {
    const auto descriptors = epoll_.Wait(kPollingTimeout, kClientEpollMaxEvents);
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
    epoll_.Modify(socket_, hash_server::EPollDirection::kReadFrom);
    const auto desc = epoll_.Wait(kPollingTimeout, kClientEpollMaxEvents);
    std::string response;
    response.reserve(kExpectedResponseSize);
    while (response.size() < kExpectedResponseSize) {
      response += socket_.Read(kExpectedResponseSize);
    }
    return response;
  }

 private:
  hash_server::TCPSocket socket_;
  hash_server::EPoll epoll_;
};

TEST_F(ServerFixture, SimpleHelloWorldTest) {
  Client client;
  client.WaitForConnection();
  std::string response = client.GetHashForString("Hello, World!");
  std::cout << "Response: " << response << "\n";
  ASSERT_EQ(response, kPreComputedHashHelloWorld);
}

TEST_F(ServerFixture, MultipleClientsTest) {
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

TEST_F(ServerFixture, DealingWithTheRestOfStringTest) {
  Client client;
  client.WaitForConnection();
  client.SendString("Hello, World!\n!dlroW ");
  auto response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorld);
  client.SendString(",olleH\n");
  response = client.ReadResponse();
  ASSERT_EQ(response, kPreComputedHashHelloWorldReversed);
}

TEST_F(ServerFixture, MultipleStringInOneMessageTest) {
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
