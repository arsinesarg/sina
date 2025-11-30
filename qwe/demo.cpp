#include "parallel_scheduler.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

// Глобальный мьютекс для синхронизации вывода
pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

void exampleTask(void* arg) {
    int taskId = *static_cast<int*>(arg);
    
    // Синхронизируем вывод
    pthread_mutex_lock(&output_mutex);
    cout << "Executing task " << taskId << endl;
    pthread_mutex_unlock(&output_mutex);
    
    sleep(1);
    
    pthread_mutex_lock(&output_mutex);
    cout << "Completed task " << taskId << endl;
    pthread_mutex_unlock(&output_mutex);
}

int main() {
    ParallelScheduler scheduler(4);
    int taskIds[10];
    
    for (int i = 0; i < 10; i++) {
        taskIds[i] = i + 1;
        scheduler.run(exampleTask, &taskIds[i]);
    }
    
    sleep(12);
    
    pthread_mutex_lock(&output_mutex);
    cout << "All tasks processed successfully" << endl;
    pthread_mutex_unlock(&output_mutex);
    
    pthread_mutex_destroy(&output_mutex);
    return 0;
}
