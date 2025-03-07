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
    Channel *newChannel = new Channel(channelName, creator);
    channelMap[channelName] = newChannel;
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

void Server::process_operation(char sign, const char &oper, int i, std::string &arg, Channel *channel){
    if (oper == 'o'){
        if (sign == '+'){
            if (clients[i]->get_buffer_size() > 3){
                // Client *newOp = find_user(clients[i]->get_cmd(3), i, channel);
                Client *newOp = find_user(arg, i, channel);
                channel->getMembers()[newOp] = true;
                puts("he is now an operator");
            }
        }
        else if (sign == '-'){
            if (clients[i]->get_buffer_size() > 3){
                // Client *removeOp = find_user(clients[i]->get_cmd(3), i, channel);
                Client *removeOp = find_user(arg, i, channel);
                channel->getMembers()[removeOp] = false;
                puts("7aydna lih operator");
            }
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
                // std::string limit = clients[i]->get_cmd(3);
                std::string limit = arg;
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
                // std::string pass = clients[i]->get_cmd(3);
                std::string pass = arg;
                puts("here");
                channel->setRequiresPass(true);
                channel->setPass(pass);
                channelAndPass[channel->getChannelName()] = pass;
            }
        }
        else if (sign == '-'){
            channel->setRequiresPass(false);
            channel->setPass(""); // hmmmm tanchof wach blan
        }
    }
    else if (oper == 't'){
        if (sign == '+')
            channel->setTopicFlag(true);
        else if (sign == '-')
            channel->setTopicFlag(false);
    }
        
}

bool Server::requiresArg(char sign, char oper){
    return (sign == '+' && oper == 'o' || (sign == '-' && oper == 'o') || (sign == '+' && oper == 'l') || (sign == '+' && oper == 'k'));
}

void  Server::check_operations(const std::string &opers, int i, Channel *channel){
    char modeSign = '+';
    std::vector<std::pair<char, char> > operations;
    std::vector<std::string> options;
    if (opers.empty()){
        // needmoreparams
        return;
    }
    for (auto it = opers.begin(); it != opers.end(); it++){
        if (*it == '+' || *it == '-')
            modeSign = *it;
        else if (modes.find(*it) != modes.end())
            operations.push_back(std::make_pair(*it, modeSign));
    }
    std::cout << clients[i]->get_buffer_size() << '\n';
    for (size_t j = 3; j < clients[i]->get_buffer_size(); j++) //  machi dima ghaykon idan hadchi mzl chwiya flawed
        options.push_back(clients[i]->get_cmd(j));
    for (auto it = operations.begin(); it != operations.end(); ){ // we dont increment here erase returns next valid iterator
        if (requiresArg(it->second,it->first)){
            try{
                if (!options.empty()){
                    std::string arg = options.front();
                    options.erase(options.begin());
                    process_operation(it->second, it->first, i, arg ,channel);
                }
            }
            catch(std::exception &e){
                std::cout << "ERROR: " << e.what() << std::endl;
            }
            it = operations.erase(it); // we should always erase not just inside the try block
            // 7it la t throwat exception l iterator aywli invalid
        }
        else{
            process_operation(it->second, it->first, i, options.front(), channel); // sawb function khra lhadok li may7tajoch args
            it++;
        }
    }
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

void    Server::change_nick_name(int i)
{
    parse_nick(i);
    //broadcast to all channels
}

void Server::invite_user(const std::string &invited, Client *sender ,const std::string &chan){
    if (channelMap[chan]->getInviteOnly()
        && !channelMap[chan]->isOperator(sender)){
            //ERR_CHANOPRIVSNEEDED
            return;
    }
    Client *cinvited = NULL;
    for (auto it = clients.begin(); it != clients.end(); it++){
        if ((*it)->get_nick_name() == invited){
            cinvited = *it;
            break;
        }
    }
    if (!cinvited){
        puts("no such nick err");
        return;
    }
    auto iter = channelMap.find(chan);
    if (iter != channelMap.end()){
        auto member = iter->second->getMembers().find(cinvited);
        if (member != iter->second->getMembers().end()){
            puts("already a member of the channel");
            return;
        }
    }
    // ila kan already fdik channel
    std::string msg = server_prefix + " you're invited to join the channel\n";
    // send_reply(clientsget_socket_fd(), msg);
// channel makaynch - user makaynch fdik channel
// invite message
// mat invitich rask
}


// bghiti tkhdm bl exceptions khs dir try catch ajmi
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
    // join &chan || join #chan not the same thing
    else if (clients[i]->get_cmd(0) == "join" || clients[i]->get_cmd(0) == "JOIN" ){ // tolower w compari
        if (clients[i]->get_buffer_size() < 2){
            clients[i]->reset();
            return;
        }
        std::string channels = clients[i]->get_cmd(1);
        if (clients[i]->get_buffer_size() > 2){
            std::string pass = clients[i]->get_cmd(2);
            extract_channels(channels, i, pass);
            // for (auto it= channelAndPass.begin();it != channelAndPass.end();it++){ debugging channel name key
            //     std::cout << "name:" << it->first << " pass: " << it->second << '\n';
            // }
        }
        else
            extract_channels(channels, i, "");
        auto it = channelAndPass.begin();
        while (it != channelAndPass.end()){  // i can use a for loop bla increment ghir khasni n3rf imta n erasi w imta la
				// 7iyd lprefix hna w hni rask bach matb9ach dirha kola mra => lblan dial &chan #chan makit7sboch kifkif
			std::string channelName = it->first;
            // std::cout << channelName << " channel name is" << '\n';
            if (!channelName.empty() && channel_exists(channelName) == false){
	            create_channel(channelName, clients[i]);
            }
            else if (channel_exists(channelName) == true){ // check invite only !!!!!!!
				if (channelMap[channelName]->getRequiresPass() == true){
                    if (channelMap[channelName]->getPass() == it->second)
                        append_user_to_channel(channelMap[channelName], clients[i]);// 9waada hna kfch member
                    else // reply dial you need a password
                    {
                        // std::cout << "pass entered was " << it->second << '\n';
                        // std::cout << "it was compared with" << channelMap[channelName]->getPass() << '\n';
                        puts("pass incorrect");
                    }
                }
                else
                    append_user_to_channel(channelMap[channelName] ,clients[i]); // ila makanch already member
                }
                it = channelAndPass.erase(it);
			}
        }
        else if (clients[i]->get_cmd(0) == "mode" || clients[i]->get_cmd(0) == "MODE"){
			if (clients[i]->get_buffer_size() < 3)
				return;
            std::string chan = clients[i]->get_cmd(1);
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
            else{
                puts("channel doesn't exist");
            }
        }
        // chi user 3ndo smia dial chi channel anfr9 binathom b #
        else if (clients[i]->get_cmd(0) == "INVITE" || clients[i]->get_cmd(0) == "invite"){
            if (clients[i]->get_buffer_size() < 3 || !check_user(i)){
                clients[i]->reset();
                return ;
            }
            std::string invited = clients[i]->get_cmd(1);
            std::string chan = clients[i]->get_cmd(2);
            invite_user(invited, clients[i], chan);
        }
        else if (clients[i]->get_cmd(0) == "TOPIC" || clients[i]->get_cmd(0) == "topic"){
            if (clients[i]->get_buffer_size() < 2) // needmoreparams
                return;
            else if (clients[i]->get_buffer_size() == 2){
                std::string name = clients[i]->get_cmd(1);
                std::string msg = channelMap[name]->getTopic();
                if (msg.empty()){
                    puts("no topic set");
                    return;
                }
	            send(clients[i]->get_socket_fd(), msg.c_str(), msg.length(), 0);
                return;
            }
            std::vector<std::string> vec = clients[i]->get_cmd_buffer();
            auto it = std::find(vec.begin(), vec.end(), ":");
            if (it != vec.end()){
                it++;
            while (it != vec.end()){
                std::string name = clients[i]->get_cmd(1);
                channelMap[name]->getTopic() + *it + ' ';
                it++;
            }
            std::string topic = channelMap[clients[i]->get_cmd(1)]->getTopic();
            topic[topic.length() - 1] = '\0';
            channelMap[clients[i]->get_cmd(1)]->setTopic(topic);
            std::cout << "topic set" << '\n';
            return;
            }
            size_t pos = clients[i]->get_buffer().find(':');
            if (pos != std::string::npos){
                std::string topic = clients[i]->get_buffer().substr(pos + 1, clients[i]->get_buffer().length() - pos - 2); // tatbdl hadchi
                std::cout << "topic is " << topic << '\n';
    // khs topic ndiro lfo9 bach fga3 l7alat nreturnih
            }
            else
                std::string topic = clients[i]->get_cmd(2);
        }
        else if (clients[i]->get_cmd(0) == "KICK" || clients[i]->get_cmd(0) == "kick"){

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


// check discord diali 7at screen tests dial mode


// join #chan1,#chan1 key key
// pk makhdamach

// check if its a member of the channel before checking if it has a password