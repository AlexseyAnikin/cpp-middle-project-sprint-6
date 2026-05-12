#include <gtest/gtest.h>

#include "queue/unbounded_queue.hpp"

TEST(UnboundedQueueTest, PopFromEmptyQueueReturnsNullopt) 
{
    dispatcher::queue::UnboundedQueue queue;

    auto task = queue.try_pop();

    EXPECT_FALSE(task.has_value());
}

TEST(UnboundedQueueTest, PushAndPopOneTask) 
{
    dispatcher::queue::UnboundedQueue queue;

    bool executed = false;

    queue.push([&executed] {
        executed = true;
    });

    auto task = queue.try_pop();

    ASSERT_TRUE(task.has_value());

    (*task)();

    EXPECT_TRUE(executed);
}

TEST(UnboundedQueueTest, PreservesFifoOrder) 
{
    dispatcher::queue::UnboundedQueue queue;

    std::vector<int> result;

    queue.push([&result] {
        result.push_back(1);
    });

    queue.push([&result] {
        result.push_back(2);
    });

    queue.push([&result] {
        result.push_back(3);
    });

    for (int i = 0; i < 3; ++i) {
        auto task = queue.try_pop();

        ASSERT_TRUE(task.has_value());

        (*task)();
    }

    EXPECT_EQ(result, std::vector<int>({1, 2, 3}));
}