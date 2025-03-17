#include "bot.hpp"

void    Bot::check_port(std::string _p)
{
    char *checker;
    _port = std::strtod(_p.c_str(), &checker);
    if (checker[0] != '\0' || _port < 1 || _port > 65535)
        throw std::runtime_error("invalid port format!!\n");
}

void Bot::parse(char **av)
{
    _ip = av[1];
    if (_ip.length() < 7 || _ip.length() > 16)
        throw std::runtime_error("invalid ip address!!\n");
    check_port(av[2]);
    _nick = "nick " + static_cast<std::string>(av[3]) + CRLF;
    _user = "user * * * *" + CRLF; // does not matter, we are not handling server to server communication
    _pass = "pass " + static_cast<std::string>(av[4]) + CRLF;
}

void    Bot::_connect()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
        throw std::runtime_error("socket creation failed\n");
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    if (connect(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
        throw std::runtime_error("failed to connect to server, please check the address and the port!!\n");
    send(_socket, _pass.c_str(), _pass.length(), 0);
    send(_socket, _nick.c_str(), _nick.length(), 0);
    send(_socket, _user.c_str(), _user.length(), 0);
    char _c[2000];
    int c = recv(_socket, _c, sizeof(_c), 0);
    _c[c] = '\0';

    std::istringstream ss(_c);
    std::string _err;
    ss >> _err; ss >> _err;
    if (_err == "464" || _err == "431" || _err == "433")
    {
        std::string _mahdi = _c;
        throw std::runtime_error(_mahdi.substr(1, _mahdi.length()));
    }
    std::cout << _c;
    while (1) 
    {
        std::string nick;
        char buffer[1024] = {0};
        int bytes = recv(_socket, buffer, sizeof(buffer), 0);
        if (bytes < 0) 
        {
            std::cerr << "server closed or some fatal error happened" << std::endl;
            close(_socket);
            break ;
        }
        buffer[bytes] = '\0';
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
    if (ac != 5)
        return (std::cerr << "invalid number of arguments\n", 1);
    Bot my_Bot;
    try
    {
       my_Bot.parse(av);
       my_Bot._connect();
    }
    catch(std::exception &e)
    {
        std::cerr << "ERROR: " << e.what();
    }
}
