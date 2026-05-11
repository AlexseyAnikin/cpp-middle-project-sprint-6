#pragma once
#include "queue/queue.hpp"

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
public:
    explicit BoundedQueue(int capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override;

private:
    std::queue<std::function<void()>> tasks_;
    std::size_t capacity_ = 0;

    std::mutex mutex_;
    std::condition_variable has_space_cv_;
};

}  // namespace dispatcher::queue