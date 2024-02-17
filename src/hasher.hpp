#pragma once

#include <openssl/sha.h>

#include <string>

namespace hash_server {
class Hasher {
 public:
  Hasher();

  void Update(const std::string &data);
  std::string Finalize();

 private:
  SHA256_CTX context_;
};
}  // namespace hash_server
