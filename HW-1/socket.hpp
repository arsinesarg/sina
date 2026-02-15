#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <sys/types.h>

namespace SimpleNet 
{

class Socket 
{
public:
    Socket();
    ~Socket();

    // Запрет копирования
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // Перемещение
    Socket(Socket&& source) noexcept;
    Socket& operator=(Socket&& source) noexcept;

    void bind(int target_port);
    void listen(int max_queue = 10);
    void connect(const std::string& remote_ip, int remote_port);
    Socket accept();

    std::vector<char> receive(size_t limit = 4096);
    ssize_t send(std::string_view payload);

private:
    // Приватный конструктор для метода accept
    explicit Socket(int handle) : handle_{handle} {}
    int handle_ = -1;
};

} // namespace SimpleNet
