#include "server.hpp"

Server::Server() : bot_status(false)
{
    server_prefix = ":ft_irc_1337 "; // to make it easy to send messages with the indecating our server
    
}

void    Server::registration_msge(int i)
{
    std::string nick = clients[i]->get_nick_name();
    taken_nick_name(i);
    std::string message = 
            ":ft_irc 001 " + nick + " :Welcome to the :ft_irc Network\r\n"
            ":ft_irc 002 " + nick + " :Your host is :ft_irc, running version version: 01\r\n"
            ":ft_irc 254 " + nick + " :channels formed\r\n" // add number of channels
            ":ft_irc 255 " + nick + " :We have " + std::to_string (clients.size() - 1) + " clients\r\n";
    std::string motd = 
        ":ft_irc 372 " + nick + " :-   __  _         _               _  _____ _____ _____ \r\n"
        ":ft_irc 372 " + nick + " :-  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |\r\n"
        ":ft_irc 372 " + nick + " :- | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / \r\n"
        ":ft_irc 372 " + nick + " :- |  _|| |_      | || |  | (__   | | ___) |___) | / /  \r\n"
        ":ft_irc 372 " + nick + " :- |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   \r\n"
        ":ft_irc 372 " + nick + " :-          |_____|                                     \r\n"
        ":ft_irc 372 " + nick + " :- irc1337 is a really cool network!\r\n"
        ":ft_irc 372 " + nick + " :- No spamming please, thank you!\r\n";
    message += motd;
    send(clients[i]->get_socket_fd(), message.c_str(), message.length(), 0);
    clients[i]->showed_messgae();
}

bool white_space(const std::string& str) 
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (std::isspace(static_cast<char>(str[i])))
            return true;
    }
    return false;
}

void    Server::check_pass(std::string _pass)
{
    if (white_space(_pass))
        throw std::runtime_error("invalid password format");
    if (_pass.length() < 3)
        throw std::runtime_error("too short passwrod");
    if (_pass.length() > 20)
        throw std::runtime_error("too long passwrod");
}

void    Server::check_port(std::string _p)
{
    for (size_t i = 0; i < _p.length();i++)
    {
        if (!std::isdigit(_p[0]))
            throw std::runtime_error("invalid port format");
    }
    char *checker;
    port = std::strtod(_p.c_str(), &checker);
    if (checker[0] != '\0' || port < 1 || port > 65535)
            throw std::runtime_error("invalid port format");
}

void Server::server_setup(std::string _port, std::string passwd)
{
    password = passwd;
    char *checker = NULL;
    check_port(_port);
    check_pass(passwd);
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket == -1)
        throw std::runtime_error("failed to create socket");
    // fcntl(server_socket, F_SETFL, O_NONBLOCK);
    sock_addr.sin_family = AF_INET; //  select the ipv4 family
    sock_addr.sin_addr.s_addr = INADDR_ANY; // chose the network interfaces will listen on
    sock_addr.sin_port = htons(port); // the port will listen on, here about the little endianne and big endianne
    int opt = 1;
    if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) || (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)) // keeping the socket alive after the program terminate (skep wait time to handle the packets in the socket)
        throw std::runtime_error("SETSOCKOPT FUNCTION FAILED");
    if (bind(server_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        close (server_socket);
        throw std::runtime_error("bind failed");
    }
    if (listen(server_socket, 100) < 0) // set the backlog = how much cleints in the queue
    {
        close (server_socket);
        throw std::runtime_error("listen failed"); // try to print the errno
    }
    Client* poll_server = new Client(); // throwed exc will catched in the main and the program wont start
    poll_server->add_server_to_poll(server_socket);
    clients.push_back(poll_server);
    _poll_fds.push_back(poll_server->get_socket_struct());
    std::cout << "server listening on port: " << port << std::endl;
}

void    Server::handle_new_client()
{
    Client *new_client = NULL;
    try
    {
        new_client = new Client;
        new_client->connect(server_socket);
        clients.push_back(new_client); // add the client to the vector
        _poll_fds.push_back(new_client->get_socket_struct());
        std::cout << "Client connected, on fd: "  << new_client->get_socket_fd() << "\r\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        delete new_client;
    }
}

void Server::handle_event_fd(int i)
{
    char buffer[500] = {0};
    int bytes = recv(clients[i]->get_socket_fd(), buffer, 499, 0); // put in the stream of the client
    if (bytes <= 0)
    {
        close (clients[i]->get_socket_fd());
        clients[i]->disconnected();
    }
    else
    {
        buffer[bytes] = '\0';
        clients[i]->append_buffer(buffer);
        if (clients[i]->get_buffer().length() > 10000)
        {
            std::string msge =  server_prefix + "input line too long\n";
            send_reply(clients[i]->get_socket_fd(), msge);
            clients[i]->reset();
            clients[i]->clear_buffer();
            return ;
        }
        if (clients[i]->cmd_end() && !clients[i]->check_all())
        {
            std::cout << clients[i]->get_buffer() << std::endl;
            while (clients[i]->get_buffer().length()) // split the request to handle bot connection
            {
                handle_cmd(i);
                clients[i]->reset();
            }
        }
        else if (clients[i]->cmd_end())
        {
            while (clients[i]->get_buffer().length())
            {
                handle_cmd_1(i);
                clients[i]->reset();
            }
        }
        clients[i]->clear_buffer();
    }
}

void Server::multiplexing_func()
{
    while (true)
    {
        int ready = poll(_poll_fds.data(), _poll_fds.size(), 0); // non-blocking poll
        if (ready == -1)
            throw std::runtime_error("poll error");
        for (size_t i = 0; i < _poll_fds.size(); i++)
        {
            if (_poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL) || !(clients[i]->check_connection())) // check if a client cut off
            {
                std::cout << "hangup or error on fd " << _poll_fds[i].fd << std::endl;
                close(clients[i]->get_socket_fd());
                _poll_fds.erase(_poll_fds.begin() + i);
                delete clients[i];
                clients.erase(clients.begin() + i);
                --i;
            }
            if (_poll_fds[i].revents & POLLIN && clients[i]->check_connection()) // if a connection to the socket requested and its writing request
            {
                if (_poll_fds[i].fd == server_socket) // new events is on the socket file desctiptor
                    handle_new_client();
                else
                    handle_event_fd(i);
            }
        }
    }
}

void    Server::send_reply(int fd, std::string message)
{
    int bytes = send(fd, message.c_str(), message.length(), 0);
    if (bytes < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            std::cerr << "an error occured while sending data\n";
    }
}

Server::~Server()
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->get_socket_fd() != -1)
            close(clients[i]->get_socket_fd());
        delete clients[i];
    }
    clients.clear();
    if (server_socket != -1)
        close(server_socket);
}
