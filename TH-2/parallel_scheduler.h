#ifndef PARALLEL_SCHEDULER_H
#define PARALLEL_SCHEDULER_H

#include <pthread.h>
#include <queue>
#include <functional>
#include <vector>

struct TaskItem {
    std::function<void(void*)> func;
    void* arg;
};

class ParallelScheduler {
    std::vector<pthread_t> threads;
    std::queue<TaskItem> taskQueue;
    pthread_mutex_t queueMutex;
    pthread_cond_t queueCond;
    bool stopFlag;

    static void* workerFunc(void* arg);

public:
    explicit ParallelScheduler(size_t numThreads);
    ~ParallelScheduler();

    void run(std::function<void(void*)> func, void* arg);
};

#endif
// TH-2
