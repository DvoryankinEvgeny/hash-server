#include "hasher.hpp"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace hash_server {

Hasher::Hasher() { SHA256_Init(&context_); }

void Hasher::Update(std::string_view data) { SHA256_Update(&context_, data.data(), data.size()); }

std::string Hasher::Finalize() {
  std::array<unsigned char, SHA256_DIGEST_LENGTH> hash{};
  SHA256_Final(hash.data(), &context_);
  std::string outputBuffer;
  outputBuffer.resize(SHA256_DIGEST_LENGTH * 2);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(outputBuffer.data() + (i * 2), "%02x", hash[i]);
  }
  SHA256_Init(&context_);
  return outputBuffer;
}

}  // namespace hash_server
