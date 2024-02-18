#include <gtest/gtest.h>

#include "hasher.hpp"
#include "precomputed_hashes.hpp"

TEST(Sha256HasherTests, EmptyStringTest) {
  const auto hasher = hash_server::CreateHasher(hash_server::kSHA256);
  auto result = hasher->Finalize();
  ASSERT_EQ(result, kPreComputedHashEmptyString);
}

TEST(Sha256HasherTests, HelloWorldTest) {
  const auto hasher = hash_server::CreateHasher(hash_server::kSHA256);
  hasher->Update("Hello, ");
  hasher->Update("World!");
  auto result = hasher->Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorld);
}

TEST(Sha256HasherTests, UpdateAfterFinalizeTest) {
  const auto hasher = hash_server::CreateHasher(hash_server::kSHA256);
  hasher->Update("Hello, World!");
  auto result = hasher->Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorld);
  hasher->Update("!dlroW ,olleH");
  result = hasher->Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorldReversed);
}