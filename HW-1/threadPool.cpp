#include "ThreadPool.hpp"

namespace SimpleNet 
{

ThreadPool::ThreadPool(size_t thread_count) : is_shutdown(false) 
{
    for (size_t i = 0; i < thread_count; ++i) 
    {
        threads_.emplace_back(
            [this] 
            {
                while (true) 
                {
                    std::unique_ptr<std::function<void()>> current_task;
                    {
                        std::unique_lock<std::mutex> lock(sync_mutex);
                        condition.wait(lock, [this] {
                            return is_shutdown || !task_queue.empty();
                        });

                        if (is_shutdown && task_queue.empty()) return;

                        current_task = std::move(task_queue.front());
                        task_queue.pop();
                    }
                    if (current_task) 
                    {
                        (*current_task)();
                    }
                }
            }
        );
    }
}

ThreadPool::~ThreadPool() 
{
    {
        std::lock_guard<std::mutex> lock(sync_mutex);
        is_shutdown = true;
    }
    condition.notify_all();
    for (auto& thread_obj : threads_) 
    {
        if (thread_obj.joinable()) thread_obj.join();
    }
}

}
