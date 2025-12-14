#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

struct Client {
    int sock;
    std::string name;
};

std::vector<Client> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const std::string& message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (auto &c : clients) {
        if (c.sock != sender_sock) {
            send(c.sock, message.c_str(), message.size(), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    delete (int*)arg;

    char buffer[BUFFER_SIZE];
    int bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        close(client_sock);
        return nullptr;
    }
    buffer[bytes] = '\0';
    std::string name(buffer);
    name.erase(name.find_last_not_of("\n") + 1);

    pthread_mutex_lock(&clients_mutex);
    clients.push_back({client_sock, name});
    pthread_mutex_unlock(&clients_mutex);

    std::string join_msg = name + " joined the chat.\n";
    broadcast(join_msg, client_sock);
    std::cout << join_msg;

    while (true) {
        bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::string msg(buffer);
        msg.erase(msg.find_last_not_of("\n") + 1);

        if (msg == "/exit") break;
        else if (msg == "/list") {
            pthread_mutex_lock(&clients_mutex);
            std::string list = "Online users: ";
            for (auto &c : clients) list += c.name + " ";
            list += "\n";
            send(client_sock, list.c_str(), list.size(), 0);
            pthread_mutex_unlock(&clients_mutex);
        } else {
            std::string full_msg = name + ": " + msg + "\n";
            broadcast(full_msg, client_sock);
            std::cout << full_msg;
        }
    }

    pthread_mutex_lock(&clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [client_sock](const Client& c){ return c.sock == client_sock; }),
        clients.end());
    pthread_mutex_unlock(&clients_mutex);

    std::string leave_msg = name + " left the chat.\n";
    broadcast(leave_msg, client_sock);
    std::cout << leave_msg;

    close(client_sock);
    return nullptr;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);

    std::cout << "Server started on port " << PORT << "\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_size);

        pthread_t tid;
        int* pclient = new int(client_sock);
        pthread_create(&tid, nullptr, handle_client, pclient);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}

