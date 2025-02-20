#include "client.hpp"

Client::Client() : auth(false), pass(false)
{
    //check the username policy and duplicate
    addr_len = sizeof(socket_addr);
    memset(&socket_addr, 0, addr_len);
}

pollfd Client::get_socket_struct()
{
    return __poll;
}

socklen_t &Client::get_socket_addr_length()
{
    return addr_len;
}

void Client::connect(int server_socket)
{
    client_socket = accept(server_socket, (struct sockaddr*)&socket_addr, &addr_len);
    if (client_socket == -1)
        throw std::runtime_error("request accepting failed");
    __poll.fd = client_socket;
    __poll.events = POLLIN;
    __poll.revents = 0;
}

int Client::get_socket_fd() const
{
    return client_socket;
}

void Client::add_server_to_poll(int server_socket)
{
    __poll.fd = server_socket;
    __poll.events = POLLIN; // tells pool which request will come
    __poll.revents = 0;
}

std::string &Client::get_cmd(int i)
{
    // if (i >= _command_buffer.size())
        // return "";
    return _command_buffer[i];
}

int Client::get_buffer_size() const
{
    // if (i >= _command_buffer.size())
        // return "";
    return _command_buffer.size();
}

std::string trim(const std::string& str) 
{
    const std::string whitespace = " \t\n\r";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, last - first + 1);
}

void Client::parse_command()
{
    std::stringstream ss(_buffer);
    std::string command;
    
    while (std::getline(ss, command, ' ')) 
    {
        std::string trimmed = trim(command);
        if (!trimmed.empty())
        {
            _command_buffer.push_back(trimmed);
        }
    }
}


void Client::append_buffer(char *res)
{
    if (!strlen(res) || (strlen(res) == 1 && res[0] == '\n'))
        return ;
    _buffer.append(res);
}

bool Client::check_pass() const
{
    return pass;
}

std::string Client::get_buffer() const
{
    return _buffer;
}

bool Client::check_auth() const
{
    return auth;
}

void Client::reset()
{
    _buffer.clear();
    _command_buffer.clear();
}

void Client::correct_pass()
{
    pass = true;
}

Client::~Client()  
{
    // close (client_socket);
}
