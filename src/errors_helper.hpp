#pragma once

#include <cstring>
#include <stdexcept>

namespace hash_server {

inline void ThrowRuntimeError(const char* message) {
  std::string error(message);
  error.append(": ");
  error.append(strerror(errno));
  throw std::runtime_error(error);
}

}  // namespace hash_server
