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
    server_prefix = ":ft_irc_1337 "; // to make it easy to send messages with the indecating our server
    auth_guide = 
    "Welcome to our humble server!" + CRLF
    "To authenticate, please follow these steps:" + CRLF
    "1. Enter the server password:" + CRLF
    "   PASS <password>" + CRLF
    "2. Choose a nickname:" + CRLF
    "   NICK <nickname>" + CRLF
    "3. Register with a username and real name:" + CRLF
    "   USER <username> 0 * :<realname>" + CRLF
    "Example:" + CRLF
    "   PASS ServerPass" + CRLF
    "   NICK JohnDoe" + CRLF
    "   USER johnd 0 * John Doe" + CRLF
    "Once authenticated, you can start using the server!" + CRLF;

}

void    Server::registration_msge(Client &_C)
{
    std::string nick = _C.get_nick_name();
    taken_nick_name(_C);
    std::string message = 
            ":ft_irc 372 " + nick + " :Welcome to the :ft_irc Network" + CRLF
            ":ft_irc 372 " + nick + " :Your host is :ft_irc, running version version: 01" + CRLF
            ":ft_irc 254 " + nick + " :channels formed" + CRLF // add number of channels , this one is on the ref
            ":ft_irc 255 " + nick + " :We have " + std::to_string (clients.size() - 1) + " clients" + CRLF; // also kayn
    std::string motd = 
        ":ft_irc 372 " + nick + " :-   __  _         _               _  _____ _____ _____ " + CRLF
        ":ft_irc 372 " + nick + " :-  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |" + CRLF
        ":ft_irc 372 " + nick + " :- | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / " + CRLF
        ":ft_irc 372 " + nick + " :- |  _|| |_      | || |  | (__   | | ___) |___) | / /  " + CRLF
        ":ft_irc 372 " + nick + " :- |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   " + CRLF
        ":ft_irc 372 " + nick + " :-          |_____|                                     " + CRLF
        ":ft_irc 372 " + nick + " :- irc1337 is a really cool network!" + CRLF
        ":ft_irc 372 " + nick + " :- No spamming please, thank you!" + CRLF;
    message += motd;
    send(_C.get_socket_fd(), message.c_str(), message.length(), 0);
    _C.showed_messgae();
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
    if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)) // keeping the socket alive after the program terminate (skep wait time to handle the packets in the socket)
        throw std::runtime_error("SETSOCKOPT FUNCTION FAILED");
    if (bind(server_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        close (server_socket);
        throw std::runtime_error("BIND FAILED");
    }
    if (listen(server_socket, 100) < 0) // set the backlog = how much cleints in the queue
    {
        close (server_socket);
        throw std::runtime_error("LISTENING FAILED"); // try to print the errno
    }
    Client* poll_server = new Client(); // throwed exc will catched in the main and the program wont start
    poll_server->add_server_to_poll(server_socket);
    clients.push_back(poll_server);
    _poll_fds.push_back(poll_server->get_socket_struct());
    std::cout << "SERVER LISTENING ON PORT: " << port << std::endl;
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
        std::cout << "CLIENT CONNECTED, ON FD: "  << new_client->get_socket_fd() << CRLF;
        send_reply(new_client->get_socket_fd(), auth_guide);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        delete new_client;
    }
}

void Server::handle_event_fd(Client &_C)
{
    char buffer[512] = {0};
    int bytes = recv(_C.get_socket_fd(), buffer, 511, 0); // put in the stream of the client
    // if (bytes <= 0)
    // {
    //     close (_C.get_socket_fd());
    //     _C.disconnected();
    // }
    // else
    {
        buffer[bytes] = '\0';
        _C.append_buffer(buffer);
        if (_C.get_buffer().length() > 600)
        {
            std::string msge =  server_prefix + " :INPUT LINE TOO LONG\n";
            send_reply(_C.get_socket_fd(), msge);
            _C.reset();
            _C.clear_buffer();
            return ;
        }
        if (_C.cmd_end() && !_C.check_all())
        {

            while (_C.get_buffer().length()) // split the request to handle bot connection
            {
                handle_cmd(_C);
                _C.reset();
            }
        }
        else if (_C.cmd_end())
        {
            while (_C.get_buffer().length())
            {
                handle_cmd_1(_C);
                _C.reset();
            }
        }
        _C.clear_buffer();
    }
}

void Server::handler(int sig)
{
    (void)sig;
    if (ins)
        ins->clean_server();
    std::exit(1);
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
            if (_poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL) || !(clients[i]->check_connection())) // check if a client cut off
            {
                std::cout << "HANGUP OR ERROR ON FD: " << _poll_fds[i].fd << std::endl;
                removeChannel();
                removeUserFromChannels(*clients[i]);
                close(clients[i]->get_socket_fd());
                _poll_fds.erase(_poll_fds.begin() + i);
                delete clients[i];
                clients.erase(clients.begin() + i);
                i--;
            }
            if (_poll_fds[i].revents & POLLIN && clients[i]->check_connection()) // if a connection to the socket requested and its writing request
            {
                if (_poll_fds[i].fd == server_socket) // new events is on the socket file desctiptor
                    handle_new_client();
                else
                    handle_event_fd(*clients[i]);
                clients[i]->reset();
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
            std::cerr << "AN ERROR OCCURED WHILE SENDING DATA\n";
    }
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
        if (channel_exists(it->second.getChannelName()) && it->second.getNumMembers() == 1)
            it = channelMap.erase(it);
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