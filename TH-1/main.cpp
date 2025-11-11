#include <iostream>
#include <vector>
#include <pthread.h>
#include <random>
#include <chrono>

struct ThreadData {
    int* arr;
    int start;
    int end;
    long long sum;
};

void* threadSum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long localSum = 0;
    for (int i = data->start; i < data->end; ++i) {
        localSum += data->arr[i];
    }
    data->sum = localSum;
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " N M\n";
        return 1;
    }

    int N = std::stoi(argv[1]);
    int M = std::stoi(argv[2]);
    if (N <= 0 || M <= 0) {
        std::cout << "non neg\n";
        return 1;
    }
    if (M > N) M = N;

    std::vector<int> arr(N);
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 100);

    for (int i = 0; i < N; ++i)
        arr[i] = dist(gen);

    auto start1 = std::chrono::high_resolution_clock::now();
    long long singleSum = 0;
    for (int i = 0; i < N; ++i)
        singleSum += arr[i];
    auto end1 = std::chrono::high_resolution_clock::now();
    double timeSingle = std::chrono::duration<double, std::milli>(end1 - start1).count();

    std::vector<pthread_t> threads(M);
    std::vector<ThreadData> data(M);

    int chunk = N / M;
    int start = 0;
    for (int i = 0; i < M; ++i) {
        data[i].arr = arr.data();
        data[i].start = start;
        data[i].end = (i == M - 1) ? N : start + chunk;
        data[i].sum = 0;
        start = data[i].end;
    }

    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < M; ++i)
        pthread_create(&threads[i], nullptr, threadSum, &data[i]);

    for (int i = 0; i < M; ++i)
        pthread_join(threads[i], nullptr);

    long long multiSum = 0;
    for (int i = 0; i < M; ++i)
        multiSum += data[i].sum;

    auto end2 = std::chrono::high_resolution_clock::now();
    double timeMulti = std::chrono::duration<double, std::milli>(end2 - start2).count();

    std::cout << "summ 1 thread: " << singleSum;
    std::cout << "time 1 thread: " << timeSingle;
    std::cout << "summ (" << M << " threads): " << multiSum;
    std::cout << "time (" << M << " threads): " << timeMulti;

    return 0;
}

