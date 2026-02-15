#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

namespace SimpleNet 
{

class ThreadPool 
{
public:
    ThreadPool(size_t thread_count);

    template<class F>
    void enqueue(F&& task) 
    {
        auto wrapper = std::make_unique<std::function<void()>>(std::forward<F>(task));
        {
            std::lock_guard<std::mutex> lock(sync_mutex);
            task_queue.push(std::move(wrapper));
        }
        condition.notify_one();
    }

    ~ThreadPool();

private:
    std::vector<std::thread> threads_;
    std::queue<std::unique_ptr<std::function<void()>>> task_queue;

    std::mutex sync_mutex;
    std::condition_variable condition;
    std::atomic<bool> is_shutdown;
};

}
