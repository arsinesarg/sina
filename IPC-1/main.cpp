#define _XOPEN_SOURCE
#include <ucontext.h>
#include <unistd.h>
#include <csignal>
#include <pwd.h>
#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

void handler(int sig, siginfo_t* si, void* context) {
    (void)sig;

    pid_t pid = si ? si->si_pid : -1;
    uid_t uid = si ? si->si_uid : (uid_t)-1;

    const char* username = "(unknown)";
    struct passwd* pw = getpwuid(uid);
    if (pw) username = pw->pw_name;

    ucontext_t* uc = (ucontext_t*)context;

#if defined(__x86_64__)
    unsigned long ip = uc->uc_mcontext.gregs[REG_RIP];
    unsigned long ax = uc->uc_mcontext.gregs[REG_RAX];
    unsigned long bx = uc->uc_mcontext.gregs[REG_RBX];
#else
    unsigned long ip = uc->uc_mcontext.gregs[REG_EIP];
    unsigned long ax = uc->uc_mcontext.gregs[REG_EAX];
    unsigned long bx = uc->uc_mcontext.gregs[REG_EBX];
#endif

    char buf[512];
    int n = snprintf(buf, sizeof(buf),
        "Received SIGUSR1 from PID %d executed by UID %d (%s)\n"
        "State of context: IP=0x%lx AX=0x%lx BX=0x%lx\n",
        pid, uid, username, ip, ax, bx);

    write(STDOUT_FILENO, buf, n);
} 

int main() {
    struct sigaction sa{};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    char buf[64];
    int n = snprintf(buf, sizeof(buf), "PID: %d\n", getpid());
    write(STDOUT_FILENO, buf, n);

    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }

    while (true) {
        sleep(10);
    }

    return 0;
}

