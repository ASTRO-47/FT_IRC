#include "server.hpp"
#include "Channel.hpp"

// check edge cases kayn chi 8 dial mode
bool Server::requiresArg(char sign, char oper){
    return ((sign == '+' && oper == 'o') || (sign == '-' && oper == 'o') || (sign == '+' && oper == 'l') || (sign == '+' && oper == 'k'));
}

void  Server::check_operations(const std::string &opers, Client &client, Channel &channel){
    char modeSign = '+';
    std::vector<std::pair<char, char> > operations;
    std::vector<std::string> options;
    if (opers.empty()){
        send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        return;
    }
    for (std::string::const_iterator it = opers.begin(); it != opers.end(); it++){
        if (*it == '+' || *it == '-')
            modeSign = *it;
        else
            operations.push_back(std::make_pair(*it, modeSign));
    }
    for (size_t j = 3; j < client.get_buffer_size(); j++) //  machi dima ghaykon idan hadchi mzl chwiya flawed
        options.push_back(client.get_cmd(j));
    for (std::vector<std::pair<char, char> >::iterator it = operations.begin(); it != operations.end(); it++){
        char charMode = it->first;
        char sign = it->second;
        if (modes.find(charMode) == modes.end()){
            send_reply(client.get_socket_fd(), ERR_UNKNOWNMODE(client.get_nick_name(), charMode));
            continue;
        }
        if (requiresArg(sign, charMode)){
                if (!options.empty()){
                    std::string arg = options.front();
                    options.erase(options.begin());
                    process_operation(sign, charMode, client, arg ,channel);
                }
                else
                    send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
            }
        else
            process_operation(sign, charMode, client, options.front(), channel); // sawb function khra lhadok li may7tajoch args
    }
}

void Server::mode_handler(Client &client){
	std::string chan = client.get_cmd(1);
    if (channel_exists(chan) == true){
        if (channelMap[chan].isOperator(&client) == true){
		    std::string oper = client.get_cmd(2);
            check_operations(oper, client, channelMap[chan]);
	    }
	    else
            send_reply(client.get_socket_fd(), ERR_CHANOPRIVSNEEDED(client.get_nick_name(), chan));
        }
    else
        send_reply(client.get_socket_fd(), ERR_NOSUCHCHANNEL(chan));
}

// mode #chan ++++hol marin 10

// unknown mode to me