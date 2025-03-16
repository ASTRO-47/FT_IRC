#include "server.hpp"
#include "Channel.hpp"

Client* Server::find_user(const std::string &user, Channel &channel){
	for (std::vector<std::pair<Client*, bool> >::iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++){
        if (it != channel.getMembers().end() && it->first && it->first->get_nick_name() == user)
            return it->first;
    }
    return NULL;
}

void Server::process_operation(char sign, const char &oper, Client &client, std::string &arg, Channel &channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (!arg.empty()){
                Client *newOp = find_user(arg, channel);
                if (newOp != NULL){
                    channel.setOperator(newOp, true);
                    std::string operation = "+o " + arg;
                    Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), operation, "MODE"), channel);
                }
                else
                    send_reply(client.get_socket_fd(), ERR_NOSUCHNICK(client.get_nick_name(), arg));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            if (!arg.empty()){
                Client *removeOp = find_user(arg, channel);
                if (removeOp != NULL){
                    channel.setOperator(removeOp, false);
                    std::string operation = "-o " + arg;
                    Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), operation, "MODE"), channel);
                }
                else
                    send_reply(client.get_socket_fd(), ERR_NOSUCHNICK(client.get_nick_name(), arg));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
    }
    else if (oper == 'i'){
        if (sign == '+'){
            channel.setInviteOnly(true);
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "+i", "MODE"), channel);
        }
        else if (sign == '-'){
            channel.setInviteOnly(false);
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-i", "MODE"), channel);
        }
    }
    else if (oper == 'l'){
        if (sign == '+'){
            if (!arg.empty()){
                std::string limit = arg;
	            std::stringstream ss(limit);
	            long userLimit = 0;
	            ss >> userLimit;
	            if (ss.fail() || !ss.eof() || (userLimit < 0 || userLimit > 2147483647)){
                    send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
                    return;
                }
                channel.setLimitSet(true);
                channel.setUserLimit(userLimit);
                std::string loper = "+l " + arg;
                Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), loper, "MODE"), channel);
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel.setLimitSet(false);
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-l", "MODE"), channel);
        }
    }
    else if (oper == 'k'){
        if (sign == '+'){
            if (!arg.empty()){
                std::string pass = arg;
                channel.setRequiresPass(true);
                channel.setPass(pass);
                std::string rep = "+k " + pass;
                Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), rep, "MODE"), channel);
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel.setRequiresPass(false);
            channel.setPass("");
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-k", "MODE"), channel);
        }
    }
    else if (oper == 't'){
        if (sign == '+'){
            channel.setTopicFlag(true);
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "+t", "MODE"), channel);
        }
        else if (sign == '-'){
            channel.setTopicFlag(false);
            Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-t", "MODE"), channel);
        }
    }   
}
