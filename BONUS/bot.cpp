#include "bot.hpp"


void Bot::parse(char **av)
{
    _port = 8080;
    _nick = "nick bot\r\n";
    _user = "user\r\n";
    _pass = "pass hello\r\n";
}

void    Bot::_connect()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
        throw std::runtime_error("socket creation failed");
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = inet_addr("10.12.7.15");
    if (connect(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) < -1)
        throw std::runtime_error("failed to connect to server");
    send(_socket, _pass.c_str(), _pass.length(), 0);
    send(_socket, _nick.c_str(), _nick.length(), 0);
    send(_socket, _user.c_str(), _user.length(), 0);
    while (1) 
    {
        char buffer[1024] = {0};
        int bytesReceived = recv(_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) 
        {
            // Handle disconnection or error
            std::cerr << "Connection lost, attempting to reconnect..." << std::endl;
            close(_socket);
            _connect(); // Attempt to reconnect
        }
        std::cout << buffer;
        // if (buffer == "bot\n")
        // {
        //     send(_socket, "hello from bot", 15, 0);
        // }
    }
}

int main(int ac, char *av[])
{
    if (ac != 5)
        return (std::cerr << "not enough parameters\n", 1);
    Bot my_Bot;
    try
    {
       my_Bot.parse(av);
       my_Bot._connect();
    }
    catch(std::exception &e)
    {
        std::cerr << "ERROR; " << e.what() << std::endl;
    }
}
