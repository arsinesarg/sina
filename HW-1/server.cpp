#include "TcpServer.hpp"
#include "Socket.hpp"
#include <iostream>
#include <string>

int main() 
{
    SimpleNet::TcpServer chat_server(1111);

    chat_server.run([](SimpleNet::Socket client_socket) 
    {
        auto raw_data = client_socket.receive();
        std::string client_message(raw_data.begin(), raw_data.end());
        std::cout << "Received: " << client_message << "\n";
        client_socket.send("Echo: " + client_message);
    });
    
    return 0;
}
