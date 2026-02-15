#include "Socket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <utility>
#include <arpa/inet.h>

namespace SimpleNet 
{

Socket::Socket() 
{
    handle_ = socket(AF_INET, SOCK_STREAM, 0);
    if (handle_ == -1) 
    {
        throw std::runtime_error("Failed to create socket");
    }
}

Socket::~Socket() 
{
    if (handle_ >= 0) 
    {
        ::close(handle_);
    }
}

Socket::Socket(Socket&& source) noexcept : handle_{source.handle_} 
{
    source.handle_ = -1;
}

Socket& Socket::operator=(Socket&& source) noexcept 
{
    if (this != &source) 
    {
        if (handle_ != -1) ::close(handle_);
        handle_ = source.handle_;
        source.handle_ = -1;
    }
    return *this;
}

void Socket::bind(int target_port) 
{
    sockaddr_in endpoint{};
    endpoint.sin_family = AF_INET;
    endpoint.sin_port = htons(target_port);
    endpoint.sin_addr.s_addr = INADDR_ANY;

    if (::bind(handle_, (struct sockaddr*)&endpoint, sizeof(endpoint)) == -1) 
    {
        throw std::runtime_error("Failed to bind to port " + std::to_string(target_port));
    }
}

void Socket::listen(int max_queue) 
{
    if (::listen(handle_, max_queue) == -1) 
    {
        throw std::runtime_error("Failed to listen on socket");
    }
}

void Socket::connect(const std::string& remote_ip, int remote_port) 
{
    sockaddr_in remote_addr{};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(remote_port);
    
    if (inet_pton(AF_INET, remote_ip.c_str(), &remote_addr.sin_addr) <= 0) 
    {
        throw std::runtime_error("Invalid IP address: " + remote_ip);
    }

    if (::connect(handle_, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) 
    {
        throw std::runtime_error("Failed to connect to " + remote_ip + ":" + std::to_string(remote_port));
    }
}

Socket Socket::accept() 
{
    int incoming_fd = ::accept(handle_, nullptr, nullptr);
    if (incoming_fd < 0) 
    {
        throw std::runtime_error("Failed to accept connection");
    }
    return Socket(incoming_fd);
}

std::vector<char> Socket::receive(size_t limit) 
{
    std::vector<char> data_chunk(limit);
    
    ssize_t bytes_read = ::recv(handle_, data_chunk.data(), data_chunk.size(), 0);
    if (bytes_read == -1) 
    {
        throw std::runtime_error("Failed to receive data");
    }
    
    data_chunk.resize(bytes_read);
    return data_chunk;
}

ssize_t Socket::send(std::string_view payload) 
{
    ssize_t bytes_written = ::send(handle_, payload.data(), payload.size(), 0);
    if (bytes_written == -1) 
    {
        throw std::runtime_error("Failed to send data");
    }
    return bytes_written;
}

} // namespace SimpleNet
