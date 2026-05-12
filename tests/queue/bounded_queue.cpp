#include <gtest/gtest.h>

#include "queue/bounded_queue.hpp"

TEST(BoundedQueueTest, PopFromEmptyQueueReturnsNullopt) 
{
    dispatcher::queue::BoundedQueue queue(2);

    auto task = queue.try_pop();

    EXPECT_FALSE(task.has_value());
}

TEST(BoundedQueueTest, PushAndPopOneTask) 
{
    dispatcher::queue::BoundedQueue queue(2);

    bool executed = false;

    queue.push([&executed] {
        executed = true;
    });

    auto task = queue.try_pop();

    ASSERT_TRUE(task.has_value());

    (*task)();

    EXPECT_TRUE(executed);
}

TEST(BoundedQueueTest, PreservesFifoOrder) 
{
    dispatcher::queue::BoundedQueue queue(3);

    std::vector<int> result;

    queue.push([&result] { result.push_back(1); });
    queue.push([&result] { result.push_back(2); });
    queue.push([&result] { result.push_back(3); });

    for (int i = 0; i < 3; ++i) {
        auto task = queue.try_pop();

        ASSERT_TRUE(task.has_value());

        (*task)();
    }

    EXPECT_EQ(result, std::vector<int>({1, 2, 3}));
}

TEST(BoundedQueueTest, ThrowsOnInvalidCapacity) 
{
    EXPECT_THROW(dispatcher::queue::BoundedQueue queue(0), std::invalid_argument);
    EXPECT_THROW(dispatcher::queue::BoundedQueue queue(-1), std::invalid_argument);
}