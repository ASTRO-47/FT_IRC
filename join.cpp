#include "server.hpp"
#include "Channel.hpp"

bool Server::channel_exists(const std::string& channelName){
    return channelMap.find(channelName) != channelMap.end();
}

std::string Server::parse_join_input(const std::string &arg, size_t &start){
    if (arg.empty() || (arg.front() != '#' && arg.front() != '&'))
        throw std::runtime_error("args not formatted correctly");
    size_t comma = arg.find(',' , start);
    std::string chan = arg.substr(start, comma - start);
    if (comma != std::string::npos)
        start = comma + 1;
    else
        start = std::string::npos;
    return chan;
}

std::string Server::parse_passwords(const std::string &passwords, size_t &start){
    if (passwords.empty())
        throw std::runtime_error("no passwords");
    size_t comma = passwords.find(',' , start);
    std::string pass = passwords.substr(start, comma - start);
    if (comma != std::string::npos)
        start = comma + 1;
    else
        start = std::string::npos;
    return pass;
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
    std::map<Client *, bool>::iterator it = channel->getMembers().find(newMember);
    if (it != channel->getMembers().end()){
        std::cout << newMember->get_nick_name() << " is already member of " << channel->getChannelName() << '\n';
        return;
    }
    if (!channel->getLimitSet() || channel->getNumMembers() < channel->getUserLimit())
        channel->appendMember(newMember);
    else
        puts("baraka ajmi"); // reply
}

void Server::join_handler(Client &client){
    std::string channels = client.get_cmd(1);
    if (client.get_buffer_size() > 2){
        std::string pass = client.get_cmd(2);
        extract_channels(channels, pass);
        // for (auto it= channelAndPass.begin();it != channelAndPass.end();it++){ debugging channel name key
        //     std::cout << "name:" << it->first << " pass: " << it->second << '\n';
        // }
    }
    else
        extract_channels(channels, "");
    std::map<std::string, std::string>::iterator it = channelAndPass.begin();
    while (it != channelAndPass.end()){  // i can use a for loop bla increment ghir khasni n3rf imta n erasi w imta la
			// 7iyd lprefix hna w hni rask bach matb9ach dirha kola mra => lblan dial &chan #chan makit7sboch kifkif
		std::string channelName = it->first;
        // std::cout << channelName << " channel name is" << '\n';
        if (!channelName.empty() && channel_exists(channelName) == false){
	        create_channel(channelName, &client);
        }
        else if (channel_exists(channelName) == true){ // check invite only !!!!!!!
			if (channelMap[channelName]->getRequiresPass() == true){
                if (channelMap[channelName]->getPass() == it->second)
                    append_user_to_channel(channelMap[channelName], &client);// 9waada hna kfch member
                else // reply dial you need a password
                {
                    // std::cout << "pass entered was " << it->second << '\n';
                    // std::cout << "it was compared with" << channelMap[channelName]->getPass() << '\n';
                    puts("pass incorrect");
                }
            }
            else
                append_user_to_channel(channelMap[channelName] , &client); // ila makanch already member
            }
            it = channelAndPass.erase(it);
		}
}