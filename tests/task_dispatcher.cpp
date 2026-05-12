#include <gtest/gtest.h>

#include "task_dispatcher.hpp"

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

TEST(TaskDispatcherTest, ExecutesScheduledTask) 
{
    std::atomic<bool> executed = false;

    {
        dispatcher::TaskDispatcher dispatcher(1);

        dispatcher.schedule(dispatcher::TaskPriority::Normal, [&executed] {
            executed = true;
        });
    }

    EXPECT_TRUE(executed.load());
}

TEST(TaskDispatcherTest, ExecutesSeveralTasks) 
{
    std::atomic<int> counter = 0;

    {
        dispatcher::TaskDispatcher dispatcher(2);

        for (int i = 0; i < 10; ++i) {
            dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter] {
                ++counter;
            });
        }
    }

    EXPECT_EQ(counter.load(), 10);
}

TEST(TaskDispatcherTest, ExecutesHighPriorityBeforeNormalWhenSingleThreaded) 
{
    std::vector<int> result;
    std::mutex mutex;

    {
        dispatcher::TaskDispatcher dispatcher(
            1,
            {
                {dispatcher::TaskPriority::High, {false, std::nullopt}},
                {dispatcher::TaskPriority::Normal, {false, std::nullopt}},
            }
        );

        dispatcher.schedule(dispatcher::TaskPriority::Normal, [&] {
            std::lock_guard lock(mutex);
            result.push_back(2);
        });

        dispatcher.schedule(dispatcher::TaskPriority::High, [&] {
            std::lock_guard lock(mutex);
            result.push_back(1);
        });

        std::this_thread::sleep_for(100ms);
    }

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
}

TEST(TaskDispatcherTest, DestructorWaitsForScheduledTasks) 
{
    std::atomic<int> counter = 0;

    {
        dispatcher::TaskDispatcher dispatcher(4);

        for (int i = 0; i < 50; ++i) {
            dispatcher.schedule(dispatcher::TaskPriority::Normal, [&counter] {
                ++counter;
            });
        }
    }

    EXPECT_EQ(counter.load(), 50);
}