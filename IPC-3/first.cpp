#include "shared_array.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t stopflag = 0;

void onint(int) {
    stopflag = 1;
}

int main() {
    signal(SIGINT, onint);

    shared_array array("array-example", 10);
    sem_t* sem = array.get_semaphore();

    size_t pos = 0;
    int value = 0;

    while (!stopflag) {
        sem_wait(sem);

        array[pos] = value;
        std::cout << "[first] array[" << pos << "] = " << value << std::endl;

        sem_post(sem);

        pos = (pos + 1) % array.size();
        value++;
        sleep(1);
    }

    return 0;
}

