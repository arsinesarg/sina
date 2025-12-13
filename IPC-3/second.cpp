#include "shared_array.h"
#include <iostream>
#include <unistd.h>

int main() {
    shared_array array("array-example", 10);
    sem_t* sem = array.get_semaphore();

    while (1) {
        sem_wait(sem);

        std::cout << "[second] read: ";
        for (size_t i = 0; i < array.size(); ++i) {
            int x = array[i];
            std::cout << x << " ";
            array[i] = x + 100;
        }
        std::cout << std::endl;

        sem_post(sem);

        sleep(2);
    }

    return 0;
}

