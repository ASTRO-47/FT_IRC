#include "server.hpp"

Server::Server() 
{
    server_prefix = ":ft_irc_1337 "; // to make it easy to send messages with the indecating our server
}

// bool check_numeric(std::string m)
// {
//     for (size_t i = 0; i , m.length();i++)
//     {
//         if (!std::isdigit(m[i]))
//             return false;
//     }
//     return true;
// }

void send_reply()
{

}

void    Server::registration_msge(Client *C)
{
    std::string nick = C->get_nick_name();
    taken_nick_name(C);

    std::string message = 
            ":ft_irc 001 " + nick + " :Welcome to the :ft_irc Network\n"
            ":ft_irc 002 " + nick + " :Your host is :ft_irc, running version version: 01\n"
            ":ft_irc 254 " + nick + " :channels formed\n"
            ":ft_irc 255 " + nick + " :We have 1 clients\n";
    std::string motd = 
        ":ft_irc 372 " + nick + " :-   __  _         _               _  _____ _____ _____ \n"
        ":ft_irc 372 " + nick + " :-  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |\n"
        ":ft_irc 372 " + nick + " :- | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / \n"
        ":ft_irc 372 " + nick + " :- |  _|| |_      | || |  | (__   | | ___) |___) | / /  \n"
        ":ft_irc 372 " + nick + " :- |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   \n"
        ":ft_irc 372 " + nick + " :-          |_____|                                     \n"
        ":ft_irc 372 " + nick + " :- irc1337 is a really cool network!\n"
        ":ft_irc 372 " + nick + " :- No spamming please, thank you!\n";
    message += motd;
    send_reply(C->get_socket_fd(), message);
    C->showed_messgae();
}

void Server::server_setup(std::string _port, std::string passwd)
{
    password = passwd;
    char *checker = NULL;
    port = std::strtod(_port.c_str(), &checker);
    if (checker[0] != '\0')
        throw std::runtime_error("invalid port format");
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
    Client* poll_server = new Client();
    poll_server->add_server_to_poll(server_socket);
    clients.push_back(poll_server);
    _poll_fds.push_back(poll_server->get_socket_struct());
    std::cout << "server listening on port: " << port << std::endl;
}

void    Server::handle_new_client()
{
    Client *new_client = new Client;
    try
    {
        new_client->connect(server_socket);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        delete new_client;
        return ;
    }
    clients.push_back(new_client); // add the client to the vector
    _poll_fds.push_back(new_client->get_socket_struct());
    std::cout << "Client connected, on fd: "  <<  new_client->get_socket_fd()  << "\n";
}

void Server::handle_event_fd(Client *C)
{
    char buffer[500];
    int bytes = recv(C->get_socket_fd(), buffer, 499, 0); // put in the stream of the client
    if (bytes <= 0)
    {
        std::cout << "client disconnected\n";
        close (C->get_socket_fd());
        // _poll_fds.erase(_poll_fds.begin());
        delete C;
        // clients.erase(clients.begin() + i);
    }
    else
    {
        buffer[bytes] = '\0';  // trim the new line at the end
        C->append_buffer(buffer);
        if (!strcmp(C->get_buffer().c_str(), "halt\n"))
            throw std::runtime_error("server stoped by a client request"); // just for nothing
        if (C->cmd_end() && !C->check_all())
            handle_cmd(C);
        else if (C->cmd_end())
            handle_cmd_1(C);
    }
}

void Server::multiplexing_func()
{
    while (true)
    {
        int ready = poll(_poll_fds.data(), _poll_fds.size(), 0);   // non-blocking poll
        if (ready == -1)
            throw std::runtime_error("poll error");
        for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end();)
        {
            if ((*it)->get_socket_struct().revents & (POLLERR | POLLHUP | POLLNVAL)) 
            {
                std::cout << "hangup or error on fd " << (*it)->get_socket_fd() << std::endl;
                close((*it)->get_socket_fd());
                // _poll_fds.erase(_poll_fds.begin() + i);
                delete *it;
                it = clients.erase(it);
            }
            else if ((*it)->get_socket_struct().revents & POLLIN) 
            {
                if ((*it)->get_socket_fd() == server_socket)
                    handle_new_client();
                else
                    handle_event_fd(*it);
                it++;
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
            std::cerr << "send function failed\n";
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
