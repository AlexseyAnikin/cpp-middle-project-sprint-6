#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <condition_variable>
#include <cstddef>

namespace dispatcher::queue {

class PriorityQueue {
public:
    explicit PriorityQueue(std::map<TaskPriority, QueueOptions> options);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue();

private:
    static std::unique_ptr<IQueue> MakeQueue(const QueueOptions& options);

private:
    std::map<TaskPriority, std::unique_ptr<IQueue>> queues_;

    std::mutex mutex_;
    std::condition_variable cv_;

    std::size_t task_count_ = 0;
    bool shutdown_requested_ = false;
};

}  // namespace dispatcher::queue