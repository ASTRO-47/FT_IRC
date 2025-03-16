#include "client.hpp"

Client::Client() : _msg(false),_nick(false), pass(false), user(false), _disconnected(true)
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
    set_ip(inet_ntoa(socket_addr.sin_addr));
    __poll.fd = client_socket;
    __poll.events = POLLIN;
    __poll.revents = 0;
    fcntl(client_socket, F_SETFL, O_NONBLOCK);
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

size_t Client::get_buffer_size() const
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
std::vector<std::string>& Client::get_cmd_buffer(){
    return _command_buffer;
}

size_t  Client::get_replys_size()
{
    return replys.size();
}

void    Client::add_message(std::string m)
{
    replys.push_back(m);
    __poll.revents = POLLOUT;
}

void    Client::set_buffer(std::string _m)
{
    _second_buffer = _m;
}

void Client::clear_buffer()
{
    _second_buffer.clear();
    _buffer.clear();
}

void Client::parse_command()
{
    int del = 2;
    size_t pos = _buffer.find(CRLF);
    if (pos == std::string::npos)
    {
        pos = _buffer.find("\n");
        del--;
    }
    std::string test = _buffer.substr(0, pos);
    std::stringstream ss(test);
    std::string command;
    while (std::getline(ss, command, ' ')) 
    {
        std::string trimmed = trim(command);
        if (!trimmed.empty())
        {
            toLower(trimmed);
            _command_buffer.push_back(trimmed);
        }
    }
    if (pos == _buffer.length() - del)
        _buffer.clear();
    else
        _buffer = _buffer.substr(pos + del, _buffer.length());


    // debug printing
    // std::cout << "[" << "buffer : " << _buffer << "]" << std::endl;
    // std::cout << '[';
    // for (std::vector<std::string>::iterator it= _command_buffer.begin();it != _command_buffer.end();it++)
    //     std::cout << *it << "," ;
    // std::cout << ']' << std::endl;
    // exit (0);
}

void    Client::send_buffer()
{
    send(client_socket, replys[0].c_str(), replys[0].length(),  0);
    replys.pop_back();

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

const std::string& Client::get_buffer() const
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
    // _buffer.clear();
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

void Client::set_nick_name(std::string _n)
{
    nick  = _n; // take one or more ??
    _nick = true;
    first = true;
}

void Client::set_user_infos()
{
    user_name = _command_buffer[1];
    host_name = _command_buffer[2];
    std::string r_name ;
    server_name = _command_buffer[3];
    for (size_t i = 4; i < get_buffer_size() ;i++)
        r_name += _command_buffer[i] + ' ';
    real_name = r_name;  // check this later
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

void Client::set_ip(std::string newIp){
    _ip = newIp;
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

bool    Client::cmd_end()
{
    if (!_buffer.length())
        return false;
    if (_buffer.back() == '\n')
    {
        _second_buffer = _buffer;
        return true;
    }
    return false;
}

std::string Client::trim_message()
{
    std::string msge;
    std::vector<std::string>::iterator it = std::find(_command_buffer.begin(), _command_buffer.end(), ":");
    if (it != _command_buffer.end())
    {
        it++;
        while (it != _command_buffer.end())
        {
            msge += *it + ' ';
            it++;
        }
        msge[msge.length() - 1] = '\0';
        return msge;
    }
    size_t pos = _second_buffer.find(':');
    if (std::string::npos != pos)
        msge = _second_buffer.substr(pos + 1, _second_buffer.length() - pos - 2);
    else
    {
        if (_command_buffer[0] == "quit")
            msge = _command_buffer[1];
        else
            msge = _command_buffer[2];
    }
    return msge;
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