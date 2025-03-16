#include "bot.hpp"

void Bot::parse(char **av) // use the arg from the user input
{
    (void)av;
    _port = 8080;
    _nick = "nick bot" + CRLF;
    _user = "user f f f f f" + CRLF;
    _pass = "pass hello" + CRLF;
}

void    Bot::_connect()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
        throw std::runtime_error("socket creation failed");
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = inet_addr("10.12.2.9"); 
    if (connect(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) < -1)
        throw std::runtime_error("failed to connect to server");
    send(_socket, _pass.c_str(), _pass.length(), 0);
    send(_socket, _nick.c_str(), _nick.length(), 0);
    send(_socket, _user.c_str(), _user.length(), 0);// check the response from the serve 
    char _b[2000];
    int b = recv(_socket, _b, sizeof(_b), 0);
    _b[b] = '\0';
    std::cout << _b << std::endl;
    while (1) 
    {
        char buffer[1024] = {0};
        int bytes = recv(_socket, buffer, sizeof(buffer), 0);
        if (bytes < 0) 
        {
            // Handle disconnection or error
            std::cerr << "Connection lost, attempting to reconnect..." << std::endl;
            close(_socket);
            _connect(); // Attempt to reconnect
        }
        buffer[bytes] = '\0'; // /urandom error
        std::string buf = static_cast<std::string>(buffer);
        std::cout << buffer << std::endl;
        size_t pos = buf.find("!");
        if (pos != std::string::npos){
            std::string nick = buf.substr(1, pos - 1);
            // std::cout << "nick is " << nick << '\n';
        }
        size_t service = buf.find("joke");
        std::string suuuu = buf.substr(service, std::string::npos);
        knt hna
        // if (buf.find(":joke") != std::string::npos || buf.find("joke") != std::string::npos){
        //     random_joke(buf);
        // }
        // else if (buf.find(":capital") != std::string::npos || buf.find("capital") != std::string::npos)
        //     capital_handler();
        // std::string imad = "privmsg imad :joke\r\n";
        // send(_socket, imad.c_str(), imad.length(), 0);
    }
}

int main(int ac, char *av[])
{
    (void)ac;
    // if (ac != 5)
    //     return (std::cerr << "not enough parameters\n", 1);
    Bot my_Bot;
    try
    {
       my_Bot.parse(av);
       my_Bot._connect();
    }
    catch(std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}