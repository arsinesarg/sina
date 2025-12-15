#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    const char* hostname = "httpforever.com";
    const char* service = "80";

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* addr_result;
    if (getaddrinfo(hostname, service, &hints, &addr_result) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress = *(sockaddr_in*)addr_result->ai_addr;

    char ipString[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &serverAddress.sin_addr, ipString, sizeof(ipString));
    std::cout << "IP address: " << ipString << "\n";

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        std::cerr << "Failed to create socket\n";
        freeaddrinfo(addr_result);
        return 1;
    }

    if (connect(socketFd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed\n";
        close(socketFd);
        freeaddrinfo(addr_result);
        return 1;
    }

    freeaddrinfo(addr_result);

    const char* httpRequest =
        "GET / HTTP/1.1\r\n"
        "Host: httpforever.com\r\n"
        "Connection: close\r\n\r\n";

    if (send(socketFd, httpRequest, strlen(httpRequest), 0) < 0) {
        std::cerr << "Send error\n";
        close(socketFd);
        return 1;
    }

    std::ofstream outputFile("httpforever.html", std::ios::binary);
    char recvBuffer[4096];
    ssize_t receivedBytes;

    while ((receivedBytes = recv(socketFd, recvBuffer, sizeof(recvBuffer), 0)) > 0) {
        outputFile.write(recvBuffer, receivedBytes);
    }

    outputFile.close();
    close(socketFd);

    std::cout << "HTTP response saved to httpforever.html\n";
    return 0;
}

