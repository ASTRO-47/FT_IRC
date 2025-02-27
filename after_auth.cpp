#include "server.hpp"
#include "Channel.hpp"

void Server::handle_prv_msge(int i)
{
    clients[i]->trim_message();
    std::string msge = server_prefix + clients[i]->get_nick_name() +  " PRIVMSG :" + clients[i]->get_message() + '\n';
    send(clients[i]->get_reciever()->get_socket_fd(), msge.c_str(), msge.length(), 0);
}

bool    Server::check_user(int i)
{
    std::string _n = clients[i]->get_cmd(1);

    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            clients[i]->set_reciever((*it));
            return true;
        }
    }
    std::string msge =  "402 " + _n +  " :No such nick/channel\n";
    send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    return false;
}
bool Server::channel_exists(const std::string& channelName){
    std::string noPrefix = channelName.substr(1);
    return channels.find(noPrefix) != channels.end();
}

void Server::create_channel(const std::string& channelName, Client *creator){
    std::string noPrefix = channelName.substr(1);
    Channel *newChannel = new Channel(noPrefix, creator, channelName.front());
    channels[noPrefix] = newChannel;
    std::cout << "---+" << channels.size() << '\n';
    return;
}

std::string Server::parse_join_input(const std::string &arg, size_t &start){
    if (arg.empty() || (arg.front() != '#' && arg.front() != '&'))
        throw std::runtime_error("wazbi");
    size_t comma = arg.find(',' , start);
    std::string chan = arg.substr(start, comma - start);
    if (comma != std::string::npos)
        start = comma + 1;
    else
        start = std::string::npos;
    return chan;
}

void    Server::handle_cmd_1(int i)
{
    clients[i]->parse_command();
    if (clients[i]->get_cmd(0) == "privmsg" || clients[i]->get_cmd(0) == "PRIVMSG")
    {
        // if () need to check for all invalid conditins
        if (!check_user(i))
        {
            clients[i]->reset();
            return ;
        }
        handle_prv_msge(i);
    }
    else if (clients[i]->get_cmd(0) == "join" || clients[i]->get_cmd(0) == "JOIN" ){
        std::string input = clients[i]->get_cmd(1);
        int j = 0;
        size_t start = 0;
        switch (j = clients[i]->get_buffer_size()){
            case 2:
                while (start != std::string::npos){
                    const std::string chan = parse_join_input(input, start);
                    std::cout << "---->" << channel_exists(chan) << '\n';
                	if (!chan.empty() && channel_exists(chan) == false)
                        create_channel(chan, clients[i]);
                    // else
                    //     mzl ma3rft
                }
				// std::string lastOrOnlyChan = input.substr(start);
				// if (!lastOrOnlyChan.empty() && !channel_exists(lastOrOnlyChan))
				// 	create_channel(lastOrOnlyChan, clients[i]);
                // break;
            // case 3 : return;
            // default: throw;
        }
    }
    clients[i]->reset();
}