#include "task_dispatcher.hpp"

#include <utility>
namespace dispatcher {

std::map<TaskPriority, queue::QueueOptions>
TaskDispatcher::DefaultQueueOptions() {
    return {
        {
            TaskPriority::High,
            queue::QueueOptions{
                .bounded = true,
                .capacity = 1000,
            },
        },
        {
            TaskPriority::Normal,
            queue::QueueOptions{
                .bounded = false,
                .capacity = std::nullopt,
            },
        },
    };
}

TaskDispatcher::TaskDispatcher(
    std::size_t thread_count,
    std::map<TaskPriority, queue::QueueOptions> queue_options)
    : queue_(std::make_shared<queue::PriorityQueue>(std::move(queue_options))),
      thread_pool_(queue_, thread_count) {}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task)
{
    queue_->push(priority, std::move(task));
}

TaskDispatcher::~TaskDispatcher() = default;
}

} // namespace dispatcher