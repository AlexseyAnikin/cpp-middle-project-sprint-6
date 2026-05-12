#include "thread_pool/thread_pool.hpp"

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(std::shared_ptr<queue::PriorityQueue> queue, std::size_t thread_count) :
                        queue_(std::move(queue))
{
    if(!queue_)
    {
        throw std::invalid_argument("ThreadPool requires queue");
    }

    if(thread_count == 0)
    {
        thread_count = 1;
    }

    workers_.reserve(thread_count);

    for(std::size_t i = 0; i < thread_count; ++i)
    {
        workers_.emplace_back(&ThreadPool::Worker, this);
    }
}

void ThreadPool::Worker()
{
    while(true)
    {
        auto task = queue_->pop();

        if(!task.has_value())
        {
            return;
        }

        try
        {
            (*task)();
        } 
        catch (...)
        {
            //Исключение из задачи не должно завершать worker потоков    
        }
    }
}

ThreadPool::~ThreadPool()
{
    queue_->shutdown();
}

} // namespace dispatcher::thread_pool