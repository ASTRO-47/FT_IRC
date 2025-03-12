#include "server.hpp"
#include "Channel.hpp"

Client* Server::find_user(const std::string &user, Channel *channel){
    for (std::map<Client *, bool>::iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); it++){
        if (it->first && it->first->get_nick_name() == user && it->second != true)
            return it->first;
    }
    return NULL;// change this later
}

void Server::process_operation(char sign, const char &oper, Client &client, std::string &arg, Channel *channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                Client *newOp = find_user(arg, channel);
                if (newOp != NULL){
                    channel->getMembers()[newOp] = true;
                    std::string operation = "+o " + arg;
                    channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), operation, "MODE"));
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
                if (removeOp != NULL){ // null check 3yan mn l a7san nchecki wach dak lkey exists
                    channel->getMembers()[removeOp] = false;
                    std::string operation = "-o " + arg;
                    channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), operation, "MODE"));
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
            channel->setInviteOnly(true);// replies ajmi
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "+i", "MODE"));
        }
        else if (sign == '-'){
            channel->setInviteOnly(false); //replies ajmi
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-i", "MODE"));
        }
    }
    else if (oper == 'l'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                // std::string limit = client.get_cmd(3);
                std::string limit = arg;
	            std::stringstream ss(limit);
	            long userLimit = 0;
	            ss >> userLimit;
	            if (!ss.fail() && ss.eof() && (userLimit < 0 || userLimit > 2147483647))
                    send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
                channel->setLimitSet(true);
                channel->setUserLimit(userLimit);
                std::string loper = "+l " + arg;
                channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), loper, "MODE"));
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel->setLimitSet(false);
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-l", "MODE"));
        }
    }
    else if (oper == 'k'){
        if (sign == '+'){
            if (client.get_buffer_size() > 3){
                // std::string pass = client.get_cmd(3);
                std::string pass = arg;
                channel->setRequiresPass(true);
                channel->setPass(pass);
                std::string &channelName = channel->getChannelName();
                for (std::vector<std::pair<std::string, std::string> >::iterator it = channelAndPass.begin(); it != channelAndPass.end(); ++it){
                    if (it->first == channelName){
                        it->second = pass;
                        std::string key =  "+k " + pass;
                        channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), key, "MODE"));
                    }
                }
            }
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), "MODE"));
        }
        else if (sign == '-'){
            channel->setRequiresPass(false);
            channel->setPass(""); // hmmmm tanchof wach blan
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-k", "MODE"));
        }
    }
    else if (oper == 't'){
        if (sign == '+'){
            channel->setTopicFlag(true);
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "+t", "MODE"));
        }
        else if (sign == '-'){
            channel->setTopicFlag(false);
            channel->broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel->getChannelName(), client.get_ip(), client.get_hostname(), "-t", "MODE"));
        }
    }   
}