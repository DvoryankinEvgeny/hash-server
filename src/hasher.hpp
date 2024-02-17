#pragma once

#include <openssl/sha.h>

#include <string_view>

namespace hash_server {
class Hasher {
 public:
  Hasher();

  void Update(std::string_view data);
  std::string Finalize();

 private:
  SHA256_CTX context_;
};
}  // namespace hash_server
