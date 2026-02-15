#include "Socket.hpp"
#include <iostream>
#include <string>

int main() 
{
    SimpleNet::Socket client_socket;
    client_socket.connect("127.0.0.1", 1111);
    client_socket.send("Hello server");

    auto raw_data = client_socket.receive();
    std::string server_response(raw_data.begin(), raw_data.end());
    std::cout << "Server responded: " << server_response << "\n";
    
    return 0;
}
