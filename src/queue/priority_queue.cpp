#include "queue/priority_queue.hpp"

#include <stdexcept>
#include <utility>
namespace dispatcher::queue {

std::unique_ptr<IQueue> PriorityQueue::MakeQueue(const QueueOptions& options)
{
    if(options.bounded)
    {
        if(!options.capacity.has_value())
        {
            throw std::invalid_argument("Bounded queue requires capacity");
        }

        return std::make_unique<BoundedQueue>(*options.capacity);
    }

    return std::make_unique<UnboundedQueue>();
}

PriorityQueue::PriorityQueue(std::map<TaskPriority, QueueOptions> options)
{
    if(options.empty())
    {
        throw std::invalid_argument("PriorityQueue requires at least one queue");
    }

    for(const auto& [priority, queue_options] : options)
    {
        queues_.emplace(priority, MakeQueue(queue_options));
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task)
{
    IQueue* selected_queue = nullptr;

    {
        std::lock_guard lock(mutex_);

        if(shutdown_requested_)
        {
            throw std::runtime_error{"Cannot push task after shutdown"};
        }

        auto it = queues_.find(priority);
        if(it == queues_.end())
        {
            throw std::invalid_argument("Unknow task priority");
        }

        selected_queue = it->second.get();
    }

    selected_queue->push(std::move(task));

    {
        std::lock_guard lock(mutex_);
        ++task_count_;
    }

    cv_.notify_one();
}

std::optional<std::function<void()>> PriorityQueue::pop()
{
    std::unique_lock lock(mutex_);

    cv_.wait(lock, [this] {
        return shutdown_requested_ || task_count_ > 0;
    });

    if(task_count_ == 0)
    {
        return std::nullopt;
    }

    for(auto& [priority, queue] : queues_)
    {
        auto task = queue->try_pop();

        if(task.has_value())
        {
            --task_count_;
            return task;
        }
    }

    return std::nullopt;
}

void PriorityQueue::shutdown()
{
    {
        std::lock_guard lock(mutex_);
        shutdown_requested_ = true;
    }

    cv_.notify_all();
}

PriorityQueue::~PriorityQueue()
{
    shutdown();
}
} // namespace dispatcher::queue