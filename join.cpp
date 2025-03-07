#include "server.hpp"
#include "Channel.hpp"

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
        channelAndPass[chan] = pass;
    }
}
// khasni n ignori ila makanch formatted correctlly
// check password if channel exists moraha addi lclient ldik channel, operator kikon endo @ 9bl
// join replies

void Server::create_channel(const std::string& channelName, Client *creator){
    Channel *newChannel = new Channel(channelName, creator);
    channelMap[channelName] = newChannel;
    return;
}

void Server::append_user_to_channel(Channel *channel, Client *newMember){
    auto it = channel->getMembers().find(newMember);
    if (it != channel->getMembers().end()){
        std::cout << newMember->get_nick_name() << " is already member of " << channel->getChannelName() << '\n';
        return;
    }
    if (!channel->getLimitSet() || channel->getNumMembers() < channel->getUserLimit())
        channel->appendMember(newMember);
    else
        puts("baraka ajmi"); // reply
}