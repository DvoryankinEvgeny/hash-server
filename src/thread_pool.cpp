#include "thread_pool.hpp"

#include <iostream>

namespace hash_server {

ThreadPool::ThreadPool(size_t num_threads) {
  for (size_t i = 0; i < num_threads; i++) {
    threads_.emplace_back([this] {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(mutex_);
          cv_.wait(lock, [this] { return !tasks_.empty() || stopped_; });
          if (stopped_) return;
          task = std::move(tasks_.front());
          tasks_.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stopped_ = true;
  cv_.notify_all();
  for (auto &thread : threads_) {
    if (thread.joinable()) thread.join();
  }
}

void ThreadPool::AddTask(std::function<void()> function) {
  {
    std::cout << "Adding task\n";
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.emplace(std::move(function));
  }
  cv_.notify_one();
}

}  // namespace hash_server
