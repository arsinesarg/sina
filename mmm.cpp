#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <cstring>

#define SHM_NAME "/my_shm"
#define MAX_LEN 256

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    int N = argc - 1;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, N * MAX_LEN);

    char *shm = (char *) mmap(
        nullptr,
        N * MAX_LEN,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    sem_t *sems = (sem_t *) mmap(
        nullptr,
        N * sizeof(sem_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANON,
        -1,
        0
    );

    for (int i = 0; i < N; i++)
        sem_init(&sems[i], 1, (i == 0));

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            sem_wait(&sems[i]);

            snprintf(shm + i * MAX_LEN, MAX_LEN, "%s", argv[i + 1]);
            std::cout << argv[i + 1] << std::endl;

            if (i + 1 < N)
                sem_post(&sems[i + 1]);

            _exit(0);
        }
    }

    for (int i = 0; i < N; i++)
        wait(nullptr);

    for (int i = 0; i < N; i++)
        sem_destroy(&sems[i]);

    munmap(sems, N * sizeof(sem_t));
    munmap(shm, N * MAX_LEN);
    shm_unlink(SHM_NAME);

    return 0;
}

