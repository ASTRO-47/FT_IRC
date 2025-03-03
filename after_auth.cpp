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
    return channelMap.find(channelName) != channelMap.end();
}

void Server::create_channel(const std::string& channelName, Client *creator){
    Channel *newChannel = new Channel(channelName, creator, channelName.front());
    channelMap[channelName.substr(1)] = newChannel;
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
// makhdmtch ga3 bdak i tat7ydo prototype

void Server::extract_channels(const std::string &chans, int i, const std::string &passwords){ 
    size_t start = 0;
    size_t start2 = 0;
    std::string pass;
	while (start != std::string::npos){
	    const std::string chan = parse_join_input(chans, start);
        if (!passwords.empty() && start2 != std::string::npos)
            pass = parse_passwords(passwords, start2);
        else
            pass = "";
        channelAndPass[chan] = pass;
    }
}
// khasni n ignori ila makanch formatted correctlly
// check password if channel exists moraha addi lclient ldik channel, operator kikon endo @ 9bl
// join replies

void Server::find_user(const std::string &user, int i, Channel *channel){
    if (clients[i]->get_nick_name() == user)
        puts("cant promote yourself"); // self promotion nono
    else { // khasni n9lb 3lih fchannel
        for (auto it = channel->getMembers().begin(); it != channel->getMembers().end(); it++){
            if (it->first->get_nick_name() == user)
                puts("he is a member of the channel");
        }
    }
}

void Server::process_operation(char sign, const char &oper, int i, Channel *channel){
    if (sign == '+' && oper == 'o'){
        if (clients[i]->get_buffer_size() > 3){
            find_user(clients[i]->get_cmd(3), i, channel);
        }
    }
}

void  Server::check_operations(const std::string &opers, int i, Channel *channel){
    char modeSign = '+';
    std::vector<char> operations;
    if (opers.empty()){
        // needmoreparams
        return;
    }
    for (auto it = opers.begin(); it != opers.end(); it++){
        if (*it == '+' || *it == '-')
            modeSign = *it;
        std::set<char>::const_iterator mode_it = modes.find(*it);
        if (mode_it != modes.end()){
            operations.push_back(*it);// lcase dial +l wla -l second arg
            process_operation(modeSign, *it, i, channel);
                // pop ila tprocessat
        }
    }
}

void Server::append_user_to_channel(Channel *channel, Client *newMember){
    channel->appendMember(newMember);
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
    else if (clients[i]->get_cmd(0) == "join" || clients[i]->get_cmd(0) == "JOIN" ){ // tolower w compari
        std::string channels = clients[i]->get_cmd(1);
        if (clients[i]->get_buffer_size() > 3){
            std::string pass = clients[i]->get_cmd(2);
            extract_channels(channels, i, pass);
            // for (auto it= channelAndPass.begin();it != channelAndPass.end();it++){ debugging channel name key
            //     std::cout << "name:" << it->first << " pass: " << it->second << '\n';
            // }
        }
        else
            extract_channels(channels, i, "");
            for (auto it = channelAndPass.begin();it != channelAndPass.end();it++){
				// 7iyd lprefix hna w hni rask bach matb9ach dirha kola mra
				std::string channelName = it->first.substr(1);
            	if (!it->first.empty() && channel_exists(channelName) == false){
	                create_channel(it->first, clients[i]);
                }
            	else if (channel_exists(channelName) == true){
					if (channelMap[channelName]->getRequiresPass() == true){
                        if (channelMap[channelName]->getPass() == it->second)
                            puts("here");// join_channel
                        else
                            puts("pass incorrect");
                    }
                    else
                        append_user_to_channel(channelMap[channelName] ,clients[i]); // ila makanch already member
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
			if (clients[i]->get_buffer_size() < 3)
				return;
            std::string chan = clients[i]->get_cmd(1).substr(1);
                // check if channel valid and user has operator role fdik channel
            if (channel_exists(chan) == true){
                if (channelMap[chan]->isOperator(clients[i]) == true){
					// what operation
					std::string oper = clients[i]->get_cmd(2);
                    check_operations(oper, i, channelMap[chan]);
					// ERR_UNKNOWNMODE
				}
				else
					puts("no permission to perform hadchi");
            }
                
        }
    clients[i]->reset();
}


//vector 
// l9it +o 
// antraiter l arg wnpoppih


// sawb chi function katreturni lik wach dak lmode valid w katseti lik wach khas param wla la


// user cannot promote himself