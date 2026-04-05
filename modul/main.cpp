#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/my_shm_example"
#define MAX_LEN 256

int main(int argc, char *argv[]) {
    int N = argc - 1;
    if (N <= 0) return 0;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MAX_LEN);
    char *shm = mmap(NULL, MAX_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sems[N];
    char sem_name[32];

    for (int i = 0; i < N; i++) {
        snprintf(sem_name, sizeof(sem_name), "/sem_%d", i);
        sem_unlink(sem_name);
        sems[i] = sem_open(sem_name, O_CREAT, 0666, i == 0 ? 1 : 0);
    }

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            sem_wait(sems[i]);
            strncpy(shm, argv[i + 1], MAX_LEN);
            shm[MAX_LEN - 1] = '\0';
            printf("%s\n", shm);
            fflush(stdout);
            if (i + 1 < N) sem_post(sems[i + 1]);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++) wait(NULL);

    for (int i = 0; i < N; i++) {
        sem_close(sems[i]);
        snprintf(sem_name, sizeof(sem_name), "/sem_%d", i);
        sem_unlink(sem_name);
    }

    munmap(shm, MAX_LEN);
    shm_unlink(SHM_NAME);

    return 0;
}

