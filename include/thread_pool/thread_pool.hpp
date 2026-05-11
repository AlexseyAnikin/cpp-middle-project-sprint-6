#pragma once

#include "queue/priority_queue.hpp"

#include <cstddef>
#include <memory>
#include <vector>
#include <thread>

namespace dispatcher::thread_pool {

class ThreadPool {
public:
  ThreadPool(std::shared_ptr<queue::PriorityQueue> queue, srd::size_t thread_count);

  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;

  ThreadPool& operator=(const ThreadPool&) = delete;

private:
  void Worker();

private:
  std::shared_ptr<queue::PriorityQueue> queue_;
  std::vector<std::jthread> workers_;
};

} // namespace dispatcher::thread_pool
