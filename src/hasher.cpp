#include "hasher.hpp"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace hash_server {

class SHA256Hasher : public Hasher {
 public:
  SHA256Hasher();

  void Update(std::string_view data) override;
  std::string Finalize() override;

 private:
  SHA256_CTX context_;
};

SHA256Hasher::SHA256Hasher() { SHA256_Init(&context_); }

void SHA256Hasher::Update(std::string_view data) { SHA256_Update(&context_, data.data(), data.size()); }

std::string SHA256Hasher::Finalize() {
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

std::unique_ptr<Hasher> CreateHasher(HasherType) { return std::make_unique<SHA256Hasher>(); }

}  // namespace hash_server
