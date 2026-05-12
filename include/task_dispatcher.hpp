#pragma once

#include <memory>

#include "queue/priority_queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <thread>

namespace dispatcher {

class TaskDispatcher {
public:
    TaskDispatcher(
        std::size_t thread_count = std::thread::hardware_concurrency(),
        std::map<TaskPriority, queue::QueueOptions> queue_options = DefaultQueueOptions()
    );

    void schedule(TaskPriority priority, std::function<void()> task);

    ~TaskDispatcher();

private:
    static std::map<TaskPriority, queue::QueueOptions> DefaultQueueOptions();

private:
    std::shared_ptr<queue::PriorityQueue> queue_;
    thread_pool::ThreadPool thread_pool_;
};

}  // namespace dispatcher