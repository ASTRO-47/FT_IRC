#include "server.hpp"
#include "Channel.hpp"

bool Server::channel_exists(const std::string& channelName){
    return channelMap.find(channelName) != channelMap.end();
}

std::string Server::parse_passwords(const std::string &passwords, size_t &start){
    // if (passwords.empty())
    //     throw std::runtime_error("no passwords");
    size_t comma = passwords.find(',' , start);
    std::string pass = passwords.substr(start, comma - start);
    if (comma != std::string::npos)
        start = comma + 1;
    else
        start = std::string::npos;
    return pass;
}

std::string Server::parse_join_input(const std::string &arg, size_t &start){
    size_t comma = arg.find(',' , start);
    std::string chan = arg.substr(start, comma - start);
    if (comma != std::string::npos)
        start = comma + 1;
    else
        start = std::string::npos;
    return chan;
}

void Server::extract_channels(const std::string &chans, const std::string &passwords){ 
    size_t start = 0;
    size_t start2 = 0;
    std::string pass;
	while (start != std::string::npos){
	    const std::string chan = parse_join_input(chans, start);
        if (!passwords.empty() && start2 != std::string::npos){
            pass = parse_passwords(passwords, start2);
        }
        else
            pass = "";
        channelAndPass.push_back(std::make_pair(chan, pass));
    }
}
// khasni n ignori ila makanch formatted correctlly
// check password if channel exists moraha addi lclient ldik channel, operator kikon endo @ 9bl
// join replies

void Server::create_channel(const std::string& channelName, Client *creator){
    channelMap[channelName] = Channel(channelName, creator);
    return;
}
    // mli ykon topic msetti nsift rpl topic
void Server::append_user_to_channel(Channel &channel, Client *newMember){
    std::map<Client *, bool>::iterator it = channel.getMembers().find(newMember);
    if (it != channel.getMembers().end()){
        send_reply(newMember->get_socket_fd(), ERR_USERONCHANNEL(newMember->get_nick_name(), channel.getChannelName(), newMember->get_nick_name()));
        return;
    }
    if (!channel.getLimitSet() || channel.getNumMembers() < channel.getUserLimit()){
        channel.appendMember(newMember);
        std::string name = channel.getChannelName();
        channel.broadcastToAllMembers(CHANNEL_JOIN(newMember->get_nick_name(), name, newMember->get_ip(), newMember->get_hostname()));
        if (channel.getTopic() == true)
            send_reply(newMember->get_socket_fd(), RPL_TOPIC(newMember->get_nick_name(), channel.getChannelName(), channel.getTopicString()));
        send_reply(newMember->get_socket_fd(), RPL_NAMREPLY(newMember->get_nick_name(), name, channel.print_members()));
	    send_reply(newMember->get_socket_fd(), RPL_ENDOFNAMES(newMember->get_nick_name(), name));// print channel modes +t
        // mli ykon topic msetti nsift rpl topic
    }
    // checki hna hadchi dial limit set houa hadak
    else
        send_reply(newMember->get_socket_fd(), ERR_CHANNELISFULL(channel.getChannelName(), newMember->get_nick_name()));
}

void Server::join_handler(Client &client){
    std::string channels = client.get_cmd(1);
    std::string pass;
    if (client.get_buffer_size() > 2)
        pass = client.get_cmd(2);
    else
        pass = "";
    extract_channels(channels, pass);
    std::vector<std::pair<std::string, std::string> >::iterator it = channelAndPass.begin();
    while (it != channelAndPass.end()){
		std::string channelName = it->first;
        std::string password = it->second;
        if (channelName.empty() || (channelName.front() != '#' && channelName.front() != '&')
            || ((channelName.front() == '#' || channelName.front() == '&') && channelName.size() == 1)){
            send_reply(client.get_socket_fd(), ERR_NOSUCHCHANNEL(channelName));
            it = channelAndPass.erase(it);
            continue;
        }
        if (channelName.empty()){
            it++;
            continue;
        }
        bool exists = channel_exists(channelName);
        if (!exists)
	        create_channel(channelName, &client);
        else {
            Channel& channel = channelMap[channelName];
            if (channel.getInviteOnly() && !channel.isInvited(&client))
                send_reply(client.get_socket_fd(), ERR_INVITEONLYCHAN(client.get_nick_name(),channelName));
			else if (channel.getRequiresPass() && channel.getPass() != password)
                send_reply(client.get_socket_fd(), ERR_BADCHANNELKEY(client.get_nick_name(), channelName));
            else
                append_user_to_channel(channel, &client);
            }
            it = channelAndPass.erase(it);
	}
}