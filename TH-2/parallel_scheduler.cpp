#include "parallel_scheduler.h"
#include <iostream>

ParallelScheduler::ParallelScheduler(size_t numThreads) : stopFlag(false) {
    pthread_mutex_init(&queueMutex, nullptr);
    pthread_cond_init(&queueCond, nullptr);
    
    threads.resize(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        pthread_create(&threads[i], nullptr, &ParallelScheduler::workerFunc, this);
    }
}

ParallelScheduler::~ParallelScheduler() {
    pthread_mutex_lock(&queueMutex);
    stopFlag = true;
    pthread_mutex_unlock(&queueMutex);
    
    pthread_cond_broadcast(&queueCond);
    
    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }
    
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueCond);
}

void ParallelScheduler::run(std::function<void(void*)> func, void* arg) {
    pthread_mutex_lock(&queueMutex);
    taskQueue.push({func, arg});
    pthread_mutex_unlock(&queueMutex);
    pthread_cond_signal(&queueCond);
}

void* ParallelScheduler::workerFunc(void* arg) {
    ParallelScheduler* scheduler = static_cast<ParallelScheduler*>(arg);
    
    while (true) {
        pthread_mutex_lock(&scheduler->queueMutex);
        
        while (scheduler->taskQueue.empty() && !scheduler->stopFlag) {
            pthread_cond_wait(&scheduler->queueCond, &scheduler->queueMutex);
        }
        
        if (scheduler->stopFlag && scheduler->taskQueue.empty()) {
            pthread_mutex_unlock(&scheduler->queueMutex);
            break;
        }
        
        TaskItem item = scheduler->taskQueue.front();
        scheduler->taskQueue.pop();
        pthread_mutex_unlock(&scheduler->queueMutex);
        
        item.func(item.arg);
    }
    
    return nullptr;
}
