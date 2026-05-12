#pragma once
#include "queue/queue.hpp"

#include <queue>
#include <mutex>
namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
public:
    explicit UnboundedQueue(int capacity = 0);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override;

private:
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;  
};

}  // namespace dispatcher::queue