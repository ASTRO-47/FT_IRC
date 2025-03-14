#include "server.hpp"
#include "Channel.hpp"

Client* Server::find_user(const std::string &user, Channel &channel){
	for (std::vector<std::pair<Client*, bool> >::iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++){
        if (it->first && it->first->get_nick_name() == user && it->second != true)
            return it->first;
    }
    return NULL;// change this later
}
// self assignment f mode +o 
void Server::process_operation(char sign, const char &oper, Client &client, std::string &arg, Channel &channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                Client *newOp = find_user(arg, channel);
                if (newOp != NULL){
                    channel.setOperator(newOp, true);
                    std::string operation = "+o " + arg;
                    channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), operation, "MODE"));
                }
                else
                    send_reply(client.get_socket_fd(), ERR_NOSUCHNICK(client.get_nick_name(), arg));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            if (client.get_buffer_size() > 3){
                Client *removeOp = find_user(arg, channel);
                if (removeOp != NULL){
                    channel.setOperator(removeOp, false);
                    std::string operation = "-o " + arg;
                    channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), operation, "MODE"));
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
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "+i", "MODE"));
        }
        else if (sign == '-'){
            channel.setInviteOnly(false);
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-i", "MODE"));
        }
    }
    else if (oper == 'l'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                std::string limit = arg;
	            std::stringstream ss(limit);
	            long userLimit = 0;
	            ss >> userLimit;
	            if (!ss.fail() && ss.eof() && (userLimit < 0 || userLimit > 2147483647))
                    send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
                channel.setLimitSet(true);
                channel.setUserLimit(userLimit);
                std::string loper = "+l " + arg;
                channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), loper, "MODE"));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel.setLimitSet(false);
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-l", "MODE"));
        }
    }
    else if (oper == 'k'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                std::string pass = arg;
                channel.setRequiresPass(true);
                channel.setPass(pass);
                channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), pass, "MODE"));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel.setRequiresPass(false);
            channel.setPass("");
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-k", "MODE"));
        }
    }
    else if (oper == 't'){
        if (sign == '+'){
            channel.setTopicFlag(true);
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "+t", "MODE"));
        }
        else if (sign == '-'){
            channel.setTopicFlag(false);
            channel.broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), "-t", "MODE"));
        }
    }   
}
