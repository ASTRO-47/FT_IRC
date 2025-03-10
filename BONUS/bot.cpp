#include "bot.hpp"

Bot::Bot()
{
    _jokes.push_back("Why don’t skeletons fight each other? They don’t have the guts!");
    _jokes.push_back("I told my wife she should embrace her mistakes. She gave me a hug.");
    _jokes.push_back("Parallel lines have so much in common. It’s a shame they’ll never meet.");
    _jokes.push_back("Why was the math book sad? It had too many problems.");
    _jokes.push_back("Why don’t programmers like nature? It has too many bugs.");
    _jokes.push_back("I told my computer I needed a break, and now it won’t stop sending me vacation ads.");
    _jokes.push_back("Why do cows have hooves instead of feet? Because they lactose.");
    _jokes.push_back("I asked the librarian if the library had any books on paranoia. She whispered, 'They're right behind you...'");
    _jokes.push_back("What did the big flower say to the little flower? 'Hey, bud!'");
    _jokes.push_back("Why did the scarecrow win an award? Because he was outstanding in his field!");
}

void Bot::parse(char **av) // use the arg from the user input
{
    _port = 8080;
    _nick = "nick bot\r\n";
    _user = "user f f f f f\r\n";
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
    send(_socket, _user.c_str(), _user.length(), 0);// check the response from the serve 
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
