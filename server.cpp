#include "server.hpp"

Server* Server::ins = NULL;

Server::Server()
{
    modes.insert('o');
    modes.insert('k');
    modes.insert('l');
    modes.insert('i');
    modes.insert('t');   
    ins = this;
    server_prefix = ":ft_irc_1337 ";

}

void    Server::registration_msge(Client &_client)
{
    std::string nick = _client.get_nick_name();
    std::string message = 
    ":ft_irc 372 " + nick + " :Welcome to the ft_irc Network" + CRLF +
    ":ft_irc 002 " + nick + " :Your host is ft_irc, running version version: 01" + CRLF;
    // ":ft_irc 254 " + nick + " :" + std::to_string(channelMap.size()) + " channels formed" + CRLF +
    // ":ft_irc 255 " + nick + " :We have " + std::to_string(clients.size() - 1) + " clients" + CRLF;

std::string motd = 
    ":ft_irc 375 " + nick + " :- ft_irc Message of the Day -" + CRLF +
    ":ft_irc 372 " + nick + " :-   __  _         _               _  _____ _____ _____ " + CRLF +
    ":ft_irc 372 " + nick + " :-  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |" + CRLF +
    ":ft_irc 372 " + nick + " :- | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / " + CRLF +
    ":ft_irc 372 " + nick + " :- |  _|| |_      | || |  | (__   | | ___) |___) | / /  " + CRLF +
    ":ft_irc 372 " + nick + " :- |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   " + CRLF +
    ":ft_irc 372 " + nick + " :-          |_____|                                     " + CRLF +
    ":ft_irc 372 " + nick + " :- irc1337 is a really cool network!" + CRLF +
    ":ft_irc 372 " + nick + " :- No spamming please, thank you!" + CRLF +
    ":ft_irc 376 " + nick + " :End of MOTD command" + CRLF;
    
message += motd;
    send(_client.get_socket_fd(), message.c_str(), message.length(), 0);
    _client.showed_messgae();
}

bool white_space(const std::string& str) 
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (std::isspace((str[i])))
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
    char *checker;
    port = std::strtod(_p.c_str(), &checker);
    if (checker[0] != '\0' || port < 1 || port > 65535)
        throw std::runtime_error("invalid port format");
}

void Server::server_setup(std::string _port, std::string passwd)
{
    password = passwd;
    check_port(_port);
    check_pass(passwd);
    memset(&sock_addr, 0, sizeof(sock_addr));
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket == -1)
        throw std::runtime_error("failed to create socket");
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(port);
    int opt = 1;
    if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1))
        throw std::runtime_error("SETSOCKOPT FUNCTION FAILED");
    if (bind(server_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        close (server_socket);
        throw std::runtime_error("BIND FAILED");
    }
    if (listen(server_socket, 124) < 0)
    {
        close (server_socket);
        throw std::runtime_error("LISTENING FAILED");
    }
    memset(&_server_struct, 0, sizeof(server_socket));
    _server_struct.fd = server_socket;
    _server_struct.events = POLLIN;
    _server_struct.revents = 0;
    _poll_fds.push_back(_server_struct);

    std::cout << "SERVER LISTENING ON PORT: " << port << std::endl;
}

void    Server::handle_new_client()
{
    Client *new_client = NULL;
    try
    {
        new_client = new Client;
        new_client->connect(server_socket);
        clients.push_back(new_client);
        _poll_fds.push_back(new_client->get_socket_struct());
        std::cout << "CLIENT CONNECTED, ON FD: "  << new_client->get_socket_fd() << CRLF;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        delete new_client;
    }
}

void Server::handle_event_fd(Client &_client)
{
    char buffer[512];

    memset(&buffer, 0, sizeof(buffer));
    int bytes = recv(_client.get_socket_fd(), buffer, 511, 0);
    if (bytes <= 0)
        return (_client.disconnected());
    else
    {
        buffer[bytes] = 0;
        _client.append_buffer(buffer);
        if (_client.get_buffer().length() > 10000)
        {
            std::string msge =  server_prefix + " :INPUT LINE TOO LONG\n";
            send_reply(_client.get_socket_fd(), msge);
            _client.reset();
            _client.clear_buffer();
            return ;
        }
        if (!_client.cmd_end())
            return ;
        if (!_client.check_all())
        {
            while (_client.get_buffer().length())
            {
                handle_cmd(_client);
                _client.reset();
            }
            _client.clear_buffer();
        }
        else if (_client.check_all())
        {
            while (_client.get_buffer().length())
            {
                handle_cmd_1(_client);
                _client.reset();
            }
        }
        _client.clear_buffer();
    }
    _client.reset();
}

void Server::handler(int sig)
{
    (void)sig;
    if (ins)
        ins->clean_server();
    std::exit(1);
}

void Server::remove_client(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->get_socket_fd() == fd)
        {
            clients[i]->disconnected();
            std::cout << "HANGUP OR ERROR ON FD: " << clients[i]->get_socket_fd() << std::endl;
            removeChannel();
            removeUserFromChannels(*clients[i]);
            _poll_fds.erase(_poll_fds.begin() + i + 1);
            delete clients[i];
            clients.erase(clients.begin() + i);
            return ;
        }
    }
}


Client* Server::find_client_by_fd(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->get_socket_fd() == fd && clients[i]->check_connection())
            return clients[i];
    }
    return NULL;
}

bool Server::check_client_connection(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->get_socket_fd() == fd)
        {
            if (clients[i]->check_connection())
                return true;
            else
                return false;
        }
    }
    return false;
}

void Server::multiplexing_func()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);

    while (true)
    {
        int ready = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (ready == -1)    
            throw std::runtime_error("POLL ERROR");
        for (size_t i = 0; i < _poll_fds.size(); i++)
        {
            if (_poll_fds[i].revents & POLLIN)
            {
                if (_poll_fds[i].fd == server_socket)
                    handle_new_client();
                else
                {
                    Client *_client = find_client_by_fd(_poll_fds[i].fd);
                    if (_client)
                        handle_event_fd(*_client);
                }
            }
            if (_poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL) || !check_client_connection(_poll_fds[i].fd))
                remove_client(_poll_fds[i].fd);
        }
    }
}

void    Server::send_reply(int fd, std::string message)
{
    size_t bytes = send(fd, message.c_str(), message.size(), 0);
    if (bytes != message.length())
        std::cerr << "AN ERROR OCCURED WHILE SENDING DATA TO CLIENT" << std::endl;
}

void    Server::clean_server()
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

Server::~Server()
{
    clean_server();
}

void Server::broadcastMsg(std::string msg){
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++){
        if ((*it)->check_connection())
            send_reply((*it)->get_socket_fd(), msg);
    }
}

void Server::removeChannel(){
    for (std::map<std::string, Channel>::iterator it = channelMap.begin(); it != channelMap.end();){
        if (channel_exists(it->second.getChannelName()) && it->second.getNumMembers() <= 1){
            it = channelMap.erase(it);
        }
        else
            it++;
    }
}

void Server::removeUserFromChannels(Client& client){
    for (std::map<std::string, Channel>::iterator it = channelMap.begin(); it != channelMap.end(); it++){
        if (channel_exists(it->first))
            it->second.removeMember(&client);
    }
}














