#pragma once
#include "queue/queue.hpp"

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
public:
    explicit UnboundedQueue(int capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override;

private:
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;  
};

}  // namespace dispatcher::queue