#include <iostream>
#include <limits>
#include <unistd.h>

bool isPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) {
            return false;
        }
    }
    return true;
}

int nth_prime(int index) {
    int found = 0;
    int value = 1;
    while (found < index) {
        value++;
        if (isPrime(value)) {
            found++;
        }
    }
    return value;
}

bool isNumber(const std::string& str) {
    for (char ch : str) {
        if (ch < '0' || ch > '9') {
            return false;
        }
    }
    return !str.empty();
}

int main() {
    int pipeParentToChild[2];
    int pipeChildToParent[2];

    if (pipe(pipeParentToChild) < 0) {
        std::cerr << "Error: pipe() failed\n";
        return 1;
    }
    if (pipe(pipeChildToParent) < 0) {
        std::cerr << "Error: pipe() failed\n";
        return 1;
    }

    pid_t childPid = fork();

    if (childPid < 0) {
        std::cerr << "Process creation failed";
        return 1;
    }

    if (childPid == 0) {
        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);

        while (true) {
            int m;
            int bytesRead = read(pipeParentToChild[0], &m, sizeof(m));

            if (bytesRead <= 0) {
                break;
            }
            if (m <= 0) {
                break;
            }

            int primeValue = nth_prime(m);

            if (write(pipeChildToParent[1], &primeValue, sizeof(primeValue)) <= 0) {
                break;
            }
        }

        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);
    }
    else {
        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);

        while (true) {
            std::string user_input;
            std::cin >> user_input;

            if (user_input == "exit") {
                int stopSignal = 0;
                write(pipeParentToChild[1], &stopSignal, sizeof(stopSignal));
                break;
            }

            if (!isNumber(user_input)) {
                std::cout << "Error: only positive numbers are accepted!\n";
                continue;
            }

            int m = 0;

            try {
                m = std::stoi(user_input);
            }
            catch (...) {
                std::cout << "Error: number is too large!\n";
                continue;
            }

            if (m <= 0) {
                std::cout << "Value must be greater than zero\n";
                continue;
            }

            if (m > 100000) {
                std::cout << "Value is too large (max allowed 100000)\n";
                continue;
            }

            if (write(pipeParentToChild[1], &m, sizeof(m)) <= 0) {
                std::cerr << "Parent: failed to write\n";
                break;
            }

            int res;
            int received = read(pipeChildToParent[0], &res, sizeof(res));

            if (received <= 0) {
                std::cerr << "Parent: failed to read from child\n";
                break;
            }

            std::cout << "prime(" << m << ") = " << res << std::endl;
        }

        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);
    }

    return 0;
}

