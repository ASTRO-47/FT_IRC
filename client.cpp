#include "client.hpp"

Client::Client() : pass(false) , _nick(false), user(false), _msg(false), _disconnected(true), first(false)
{
    addr_len = sizeof(socket_addr); 
    memset(&socket_addr, 0, addr_len);
}

pollfd &Client::get_socket_struct()
{
    return __poll;
}

void   Client::disconnected()
{
    _disconnected = false;
}

bool    Client::check_connection()
{
    return _disconnected;
}

socklen_t &Client::get_socket_addr_length()
{ 
    return addr_len;
}

void Client::connect(int server_socket)
{
    client_socket = accept(server_socket, (struct sockaddr*)&socket_addr, &addr_len);
    if (client_socket < 0)
        throw std::runtime_error("can not add new client at this time");
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
    __poll.events = POLLIN; // tells poll() which request will come
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

size_t  Client::get_replys_size()
{
    return replys.size();
}

void    Client::add_message(std::string m)
{
    replys.push_back(m);
}
void Client::parse_command()
{
    std::stringstream ss(_buffer);
    std::string command;
    
    while (std::getline(ss, command, ' ')) 
    {
        std::string trimmed = trim(command);
        if (!trimmed.empty())
            _command_buffer.push_back(trimmed);
    }
    // std::cout << _buffer << ']' << std::endl;
    // debug printing
    // std::cout << '[';
    // for (auto it= _command_buffer.begin();it != _command_buffer.end();it++)
    //     std::cout << *it << "," ;
    // std::cout << ']' << std::endl;
}


void Client::append_buffer(std::string res)
{
    if (!res.length() || ((res.length() == 1 && res[0] == '\n')) || (res.length() == 2 && res[0] == '\r' && res[1] == '\n'))
        return ;
    _buffer.append(res);
}

bool Client::check_pass() const
{
    return pass;
}

bool Client::check_first_nick()
{
    return first;
}

bool Client::check_nick() const
{
    return _nick;
}

std::string Client::get_buffer() const
{
    return _buffer;
}

bool Client::check_all() const
{
    if (pass && _nick && user)
        return true;
    return false;
}

void Client::reset()
{
    _buffer.clear();
    _command_buffer.clear();
    _message.clear();
}

void Client::correct_pass()
{
    pass = true;
}

void Client::wrong_pass()
{
    pass = false;
}

void Client::set_nick_name()
{
    nick  = _command_buffer[1]; // take one or more ??
    _nick = true;
    first = true;
}

void Client::set_user_infos()
{
    // user_name = _command_buffer[1];
    // host_name = _command_buffer[2];
    // std::string r_name ;
    // server_name = _command_buffer[3];
    // for (int i = 4; i < _command_buffer.size();i++)
    //     r_name += _command_buffer[i] + ' ';
    // real_name = r_name;
    user = true;
}

std::string Client::get_nick_name() const
{
    return nick;
}

std::string Client::get_hostname() const
{
    return host_name;
}

std::string Client::get_ip() const{
    return _ip;
}

void    Client::showed_messgae()
{
    _msg = true;
}

bool Client::check_message() const
{
    return _msg;
}

bool    Client::cmd_end() const
{
    if ((_buffer[_buffer.length() - 1] == '\n'  && _buffer[_buffer.length() - 2] == '\r' ) || _buffer[_buffer.length() - 1] == '\n')
        return true;
    return false;
}

void Client::trim_message()
{
    std::vector<std::string>::iterator it = std::find(_command_buffer.begin(), _command_buffer.end(), ":");
    if (it != _command_buffer.end())
    {
        it++;
        while (it != _command_buffer.end())
        {
            _message += *it + ' ';
            it++;
        }
        _message[_message.length() - 1] = '\0';
        return ;
    }
    size_t pos = _buffer.find(':');
    if (std::string::npos != pos)
    {
        _message = _buffer.substr(pos + 1, _buffer.length() - pos - 2);
        // std::cout << _message << ']' << std::endl;
    }
    else
    {
        _message = _command_buffer[2];
    }
}

std::string Client::get_message() const
{
    return _message;
}

Client* Client::get_reciever() const
{
    return _reciever;
}

void Client::set_reciever(Client* _rec)
{
    _reciever = _rec;
}

Client::~Client()
{
    // close (client_socket);
}
