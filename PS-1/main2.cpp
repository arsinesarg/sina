#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

void do_command(char **argv) {
    struct timeval start, end;
    gettimeofday(&start, nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp failed");
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    gettimeofday(&end, nullptr);

    double duration = (end.tv_sec - start.tv_sec) +
                      (end.tv_usec - start.tv_usec) / 1e6;

    if (WIFEXITED(status))
        std::cout << "Command completed with " << WEXITSTATUS(status)
                  << " exit code and took " << duration << " seconds.\n";
    else if (WIFSIGNALED(status))
        std::cout << "Command terminated by signal " << WTERMSIG(status)
                  << " and took " << duration << " seconds.\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./do-command <command> [args...]\n";
        return 1;
    }

    char **cmd = new char*[argc];
    for (int i = 1; i < argc; i++)
        cmd[i - 1] = argv[i];
    cmd[argc - 1] = nullptr;

    do_command(cmd);

    delete[] cmd;
    return 0;
}

