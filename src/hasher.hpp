/**
 * @file hasher.hpp
 * @brief Defines the Hasher class and related types for hashing data.
 */

#pragma once

#include <openssl/sha.h>

#include <memory>
#include <string_view>

namespace hash_server {

/**
 * @enum HasherType
 * @brief Enumeration for the types of hashers.
 */
enum HasherType {
  kSHA256,  ///< SHA-256 hasher.
};

/**
 * @class Hasher
 * @brief This class provides an interface for hashing data.
 */
class Hasher {
 public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~Hasher() = default;

  /**
   * @brief Updates the hasher with data.
   * @param data The data to update the hasher with.
   */
  virtual void Update(std::string_view data) = 0;

  /**
   * @brief Finalizes the hasher and returns the hash.
   * @return The hash.
   */
  virtual std::string Finalize() = 0;
};

/**
 * @brief Creates a hasher of the specified type.
 * @param type The type of the hasher.
 * @return A unique_ptr to the hasher.
 */
std::unique_ptr<Hasher> CreateHasher(HasherType type);

}  // namespace hash_server