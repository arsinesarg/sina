#include <iostream>
#include <cstdlib> // для atoi

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " a b c\n";
        return 1;
    }

    int a = std::atoi(argv[1]);
    int b = std::atoi(argv[2]);
    int c = std::atoi(argv[3]);

    int min = a;
    int max = a;

    if (b < min) min = b;
    if (c < min) min = c;

    if (b > max) max = b;
    if (c > max) max = c;

    std::cout << "min-" << min << ", max-" << max << std::endl;

    return 0;
}