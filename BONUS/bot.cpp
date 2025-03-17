#include "bot.hpp"

void    Bot::check_port(std::string _p)
{
    char *checker;
    _port = std::strtod(_p.c_str(), &checker);
    if (checker[0] != '\0' || _port < 1 || _port > 65535)
        throw std::runtime_error("invalid port format");
}

void Bot::parse(char **av) // use the arg from the user input
{
    _ip = av[1];
    check_port(av[2]);
    _nick = "nick " + static_cast<std::string>(av[3]) + CRLF;
    _user = "user f f f f f" + CRLF;
    _pass = "pass " + static_cast<std::string>(av[4]) + CRLF;
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
    char _c[2000];
    int c = recv(_socket, _c, sizeof(_c), 0);
    _c[c] = '\0';
    std::cout << _c << std::endl;
    while (1) 
    {
        std::string nick;
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
        size_t pos = buf.find("!");
        if (pos != std::string::npos){
            nick = buf.substr(1, pos - 1);
        }
        int i = which_service(buf);
        switch(i){
            case 1:
            random_joke(nick);
            break;
            case 2:
            capital_handler(buf, nick);
            break;
            default:
			std::string s = "privmsg " + nick + " :ERROR! Args not formatted correctly" + "\r\n";
            send(_socket, s.c_str(), s.length(), 0);
        }
    }
}

int main(int ac, char *av[])
{
    (void)ac;
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
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}


// ./bot localhost 8080 nick pass