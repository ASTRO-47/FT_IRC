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
    return channelMap.find(noPrefix) != channelMap.end();
}

void Server::create_channel(const std::string& channelName, Client *creator){
    std::string noPrefix = channelName.substr(1);
    Channel *newChannel = new Channel(noPrefix, creator, channelName.front());
    channelMap[noPrefix] = newChannel;
    return;
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
// check edge cases kayn chi 8
void Server::extract_channels(const std::string &chans, int i, const std::string &passwords){
    size_t start = 0;
    size_t start2 = 0;
    std::string pass;
	while (start != std::string::npos){
	    const std::string chan = parse_join_input(chans, start);
        if (start2 != std::string::npos && !passwords.empty())
            pass = parse_passwords(passwords, start2);
        else
            pass = "";
        channelAndPass[chan] = pass;
    }
}
// khasni n ignori ila makanch formatted correctlly
// Pseudocode for a Clean Structure
// Extract channels and keys from the input.
// Loop through each channel:
// Normalize the name (keep # or &).
// Get the corresponding key (if any).
// Check if the channel exists.
// If new, create it.
// If it exists, validate key (if required).
// Add client to the channel.
// Send appropriate JOIN responses to the client.

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
        std::string channels = clients[i]->get_cmd(1);
        if (clients[i]->get_cmd(2).empty() == false){
            std::string pass = clients[i]->get_cmd(2);
            extract_channels(channels, i, pass);
            // for (auto it= channelAndPass.begin();it != channelAndPass.end();it++){ debugging channel name key
            //     std::cout << "name:" << it->first << " pass: " << it->second << '\n';
            // }
        }
        else
            extract_channels(channels, i, NULL);
            for (auto it = channelAndPass.begin();it != channelAndPass.end();it++){
            	if (!it->first.empty() && channel_exists(it->first) == false)
	                create_channel(it->first, clients[i]);
            	else if (channel_exists(it->first) == true){
					if (channelMap[it->first]->getRequiresPass() == true)
                        if (channelMap[it->first]->getPass() == channelAndPass[it->second])
                            puts("here");// join_channel
                        // else
                            // pass incorrect
                	// check if it requires a password
                }
			}
                // }
				// std::string lastOrOnlyChan = input.substr(start);
				// if (!lastOrOnlyChan.empty() && !channel_exists(lastOrOnlyChan))
				// 	create_channel(lastOrOnlyChan, clients[i]);
                // break;
            // case 3: // hna best case scenario houa join #foo bar
            // default: //hna an9lb 3la channel wlpass w nignori dakchi lakhr
            // default: throw std::runtime_error("wazbi hhhhh"); makhsnich nthrowi 7it y9d ykon khdam dakchi
        }
        else if (clients[i]->get_cmd(0) == "mode" || clients[i]->get_cmd(0) == "MODE"){
            
        }
    clients[i]->reset();
}