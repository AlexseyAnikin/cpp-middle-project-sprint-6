#include "queue/bounded_queue.hpp"

#include <stdexcept>
#include <utility>
#include <mutex>

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(int capacity)
{
    if(capacity <= 0)
    {
        throw std::invalid_argument("BoundedQueue capacity must be positive");
    }

    capacity_ = static_cast<std::size_t>(capacity);
}

void BoundedQueue::push(std::function<void()> task)
{
    std::unique_lock lock(mutex_);

    has_space_cv.wait(lock, [this]
    {
        return task_.size() < capacity_;
    });

    tasks_.push(std::move(task));
}

std::optional<std::function<void()>> BoundedQueue::try_pop()
{
    std::lock_guard lock(mutex_);

    if(tasks_.empty())
    {
        return std::nullopt;
    }

    auto task = std::move(tasks_.front());
    tasks_.pop();

    has_space_cv.notify_one();

    return task;
}

BoundedQueue::~BoundedQueue()
{
    has_space_cv.notify_all();
}

} // namespace dispatcher::queue