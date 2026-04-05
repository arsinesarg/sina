#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/my_shm"
#define MAX_LEN 256

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    int N = argc - 1;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, N * MAX_LEN);

    char *shm = mmap(NULL, N * MAX_LEN,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED, shm_fd, 0);

    sem_t *sems = mmap(NULL, N * sizeof(sem_t),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS,
                       -1, 0);

    for (int i = 0; i < N; i++)
        sem_init(&sems[i], 1, (i == 0) ? 1 : 0);

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            sem_wait(&sems[i]);
            snprintf(shm + i * MAX_LEN, MAX_LEN, "%s", argv[i + 1]);
            printf("%s\n", argv[i + 1]);
            fflush(stdout);
            if (i + 1 < N)
                sem_post(&sems[i + 1]);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++)
        wait(NULL);

    for (int i = 0; i < N; i++)
        sem_destroy(&sems[i]);

    munmap(sems, N * sizeof(sem_t));
    munmap(shm, N * MAX_LEN);
    shm_unlink(SHM_NAME);

    return 0;
}

