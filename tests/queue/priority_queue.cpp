#include <gtest/gtest.h>

#include "queue/priority_queue.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

TEST(PriorityQueueTest, PopBlocksUntilTaskIsPushed) 
{
    dispatcher::queue::PriorityQueue queue({
        {dispatcher::TaskPriority::High, {false, std::nullopt}},
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
    });

    std::atomic<bool> task_received = false;

    std::jthread worker([&] {
        auto task = queue.pop();

        ASSERT_TRUE(task.has_value());

        (*task)();

        task_received = true;
    });

    std::this_thread::sleep_for(50ms);

    EXPECT_FALSE(task_received.load());

    queue.push(dispatcher::TaskPriority::Normal, [] {});

    worker.join();

    EXPECT_TRUE(task_received.load());
}

TEST(PriorityQueueTest, HighPriorityTaskRunsBeforeNormalPriorityTask) 
{
    dispatcher::queue::PriorityQueue queue({
        {dispatcher::TaskPriority::High, {false, std::nullopt}},
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
    });

    std::vector<int> result;

    queue.push(dispatcher::TaskPriority::Normal, [&result] {
        result.push_back(2);
    });

    queue.push(dispatcher::TaskPriority::High, [&result] {
        result.push_back(1);
    });

    auto first = queue.pop();
    ASSERT_TRUE(first.has_value());
    (*first)();

    auto second = queue.pop();
    ASSERT_TRUE(second.has_value());
    (*second)();

    EXPECT_EQ(result, std::vector<int>({1, 2}));
}

TEST(PriorityQueueTest, ShutdownUnblocksPopAndReturnsNullopt) 
{
    dispatcher::queue::PriorityQueue queue({
        {dispatcher::TaskPriority::High, {false, std::nullopt}},
        {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
    });

    std::atomic<bool> pop_finished = false;

    std::jthread worker([&] {
        auto task = queue.pop();

        EXPECT_FALSE(task.has_value());

        pop_finished = true;
    });

    std::this_thread::sleep_for(50ms);

    EXPECT_FALSE(pop_finished.load());

    queue.shutdown();

    worker.join();

    EXPECT_TRUE(pop_finished.load());
}

TEST(PriorityQueueTest, ThrowsOnUnknownPriority) 
{
    dispatcher::queue::PriorityQueue queue({
        {dispatcher::TaskPriority::High, {false, std::nullopt}},
    });

    EXPECT_THROW(
        queue.push(dispatcher::TaskPriority::Normal, [] {}),
        std::invalid_argument
    );
}