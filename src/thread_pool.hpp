/**
 * @file thread_pool.hpp
 * @brief Defines the ThreadPool class for managing a pool of threads.
 */

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace hash_server {

/**
 * @class ThreadPool
 * @brief This class provides methods for managing a pool of threads.
 */
class ThreadPool {
 public:
  /**
   * @brief Constructor that takes the number of threads in the pool.
   * @param num_threads The number of threads in the pool.
   */
  explicit ThreadPool(int num_threads);

  /**
   * @brief Destructor. Stops all threads and waits for them to finish.
   */
  ~ThreadPool();

  /**
   * @brief Adds a task to the pool.
   * @param function The task to add to the pool.
   */
  void AddTask(std::function<void()> function);

 private:
  /**
   * @brief The threads in the pool.
   */
  std::vector<std::thread> threads_;

  /**
   * @brief The tasks to be executed by the threads.
   */
  std::queue<std::function<void()>> tasks_;

  /**
   * @brief A mutex for synchronizing access to the tasks.
   */
  std::mutex mutex_;

  /**
   * @brief A condition variable for notifying threads of new tasks.
   */
  std::condition_variable cv_;

  /**
   * @brief A flag indicating whether the pool is stopped.
   */
  bool stopped_ = false;
};

}  // namespace hash_server