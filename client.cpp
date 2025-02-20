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

void Client::append_buffer(char *res)
{
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
}

void Client::correct_pass()
{
    pass = true;
}

Client::~Client()  
{
    // close (client_socket);
}
