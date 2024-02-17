#include <gtest/gtest.h>

#include "hasher.hpp"
#include "precomputed_hashes.hpp"

TEST(Sha256HasherTests, EmptyStringTest) {
  hash_server::Hasher hasher;
  auto result = hasher.Finalize();
  ASSERT_EQ(result, kPreComputedHashEmptyString);
}

TEST(Sha256HasherTests, HelloWorldTest) {
  hash_server::Hasher hasher;
  hasher.Update("Hello, ");
  hasher.Update("World!");
  auto result = hasher.Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorld);
}

TEST(Sha256HasherTests, UpdateAfterFinalizeTest) {
  hash_server::Hasher hasher;
  hasher.Update("Hello, World!");
  auto result = hasher.Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorld);
  hasher.Update("!dlroW ,olleH");
  result = hasher.Finalize();
  ASSERT_EQ(result, kPreComputedHashHelloWorldReversed);
}