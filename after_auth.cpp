#include "server.hpp"
#include "Channel.hpp"

void Server::handle_prv_msge(int i)
{
    clients[i]->trim_message();
    std::string msge = ":" + clients[i]->get_nick_name() +"!~" + clients[i]->get_nick_name() + "@197.23.30.146" +  " PRIVMSG " +clients[i]->get_reciever()->get_nick_name() + " :" + clients[i]->get_message() + '\n';
    send_private_message(clients[i],  msge);
}

void Server::send_private_message(Client *sender, std::string msge)
{
    send_reply(sender->get_reciever()->get_socket_fd(), msge);

}

bool    Server::check_user(int i)
{
    std::string _n = clients[i]->get_cmd(1);

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            clients[i]->set_reciever((*it));
            return true;
        }
    }
    std::string msge = server_prefix + "402 " + _n +  " :No such nick/channel\n";
    send_reply(clients[i]->get_socket_fd(), msge);
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

Client* Server::find_user(const std::string &user, int i, Channel *channel){
    if (clients[i]->get_nick_name() == user)
        puts("cant promote yourself"); // self promotion nono
    else { // khasni n9lb 3lih fchannel
        for (auto it = channel->getMembers().begin(); it != channel->getMembers().end(); it++){
            if (it->first->get_nick_name() == user)
                return it->first;
        }
    }
    return NULL;// change this later
}

void Server::process_operation(char sign, const char &oper, int i, Channel *channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (clients[i]->get_buffer_size() > 3){
                find_user(clients[i]->get_cmd(3), i, channel);
            }
        }
        else if (sign == '-'){
            
        }
    }
    else if (oper == 'i'){
        if (sign == '+')
            channel->setInviteOnly(true);// replies ajmi
        else if (sign == '-')
            channel->setInviteOnly(false); //replies ajmi
    }
    else if (oper == 'l'){
        if (sign == '+'){
            if (clients[i]->get_buffer_size() > 3){
                std::string limit = clients[i]->get_cmd(3);
	            std::stringstream ss(limit);
	            long userLimit = 0;
	            ss >> userLimit;
	            if (!ss.fail() && ss.eof() && (userLimit < 0 || userLimit > 2147483647))
                    puts("error number");
                channel->setLimitSet(true);
                channel->setUserLimit(userLimit);
            }
        }
        else if (sign == '-'){
            channel->setLimitSet(false);
        }
    }
    else if (oper == 'k'){
        if (sign == '+'){
            if (clients[i]->get_buffer_size() > 3){
                std::string pass = clients[i]->get_cmd(3);
                channel->setRequiresPass(true);
                channel->setPass(pass);
            }
        }
        else if (sign == '-'){
            channel->setRequiresPass(false);
            channel->setPass(NULL); // hmmmm tanchof wach blan
        }
    }
    else if (oper == 't'){
        if (sign == '+')
            channel->setTopicFlag(true);
        else if (sign == '-')
            channel->setTopicFlag(false);
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
    if (!channel->getLimitSet() || channel->getNumMembers() < channel->getUserLimit())
        channel->appendMember(newMember);
    else
        puts("baraka ajmi"); // reply
}

void    Server::change_nick_name(int i)
{
    parse_nick(i);
    //broadcast to all channels
}

void    Server::handle_cmd_1(int i)
{
    std::string msge;
    clients[i]->parse_command();
    if (clients[i]->get_cmd(0) == "nick" || clients[i]->get_cmd(0) == "NICK")
        change_nick_name(i);
    else if (clients[i]->get_cmd(0) == "privmsg" || clients[i]->get_cmd(0) == "PRIVMSG")
    {
        if (clients[i]->get_buffer_size() == 1)
        {
            msge = server_prefix + "411 " + clients[i]->get_nick_name() + " :No recipient given (PRIVMSG)\n";
            send_reply(clients[i]->get_socket_fd(), msge);
            clients[i]->reset();
            return ;
        }
        if (clients[i]->get_buffer_size() == 2)
        {
            msge = server_prefix + "412 " + clients[i]->get_nick_name() + " :No text to send\n";
            send_reply(clients[i]->get_socket_fd(), msge);
            clients[i]->reset();
            return ;
        }
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
                        else // reply dial you need a password
                            puts("pass incorrect"); std::cout << "dakchi lidkhl" <<it->second << '\n';
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
    else if (clients[i]->get_cmd(0) != "pong" && clients[i]->get_cmd(0) != "PONG" )
    {
        msge = server_prefix + "421 " + clients[i]->get_cmd(0) +  ": unkown command\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    clients[i]->reset();
}


//vector 
// l9it +o 
// antraiter l arg wnpoppih


// sawb chi function katreturni lik wach dak lmode valid w katseti lik wach khas param wla la


// user cannot promote himself
