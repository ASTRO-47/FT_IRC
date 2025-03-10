#include "server.hpp"
#include "Channel.hpp"

// check edge cases kayn chi 8 dial mode
bool Server::requiresArg(char sign, char oper){
    return ((sign == '+' && oper == 'o') || (sign == '-' && oper == 'o') || (sign == '+' && oper == 'l') || (sign == '+' && oper == 'k'));
}

void  Server::check_operations(const std::string &opers, Client &client, Channel *channel){
    char modeSign = '+';
    std::vector<std::pair<char, char> > operations;
    std::vector<std::string> options;
    if (opers.empty()){
        // needmoreparams
        return;
    }
    for (std::string::const_iterator it = opers.begin(); it != opers.end(); it++){
        if (*it == '+' || *it == '-')
            modeSign = *it;
        else if (modes.find(*it) != modes.end())
            operations.push_back(std::make_pair(*it, modeSign));
    }
    // std::cout << client.get_buffer_size() << '\n';
    for (size_t j = 3; j < client.get_buffer_size(); j++) //  machi dima ghaykon idan hadchi mzl chwiya flawed
        options.push_back(client.get_cmd(j));
    for (std::vector<std::pair<char, char> >::iterator it = operations.begin(); it != operations.end(); ){ // we dont increment here erase returns next valid iterator
        if (requiresArg(it->second,it->first)){
            try{
                if (!options.empty()){
                    std::string arg = options.front();
                    options.erase(options.begin());
                    process_operation(it->second, it->first, client, arg ,channel);
                }
            }
            catch(std::exception &e){
                std::cout << "ERROR: " << e.what() << std::endl;
            }
            it = operations.erase(it); // we should always erase not just inside the try block
            // 7it la t throwat exception l iterator aywli invalid
        }
        else{
            process_operation(it->second, it->first, client, options.front(), channel); // sawb function khra lhadok li may7tajoch args
            it++;
        }
    }
}

void Server::mode_handler(Client &client){
	std::string chan = client.get_cmd(1);
    // check if channel valid and user has operator role fdik channel
    if (channel_exists(chan) == true){
        if (channelMap[chan.substr(1)]->isOperator(&client) == true){
		// what operation
		std::string oper = client.get_cmd(2);
            check_operations(oper, client, channelMap[chan.substr(1)]);
		// ERR_UNKNOWNMODE
	}
	else
		puts("no permission to perform hadchi");
    }
    else{
        puts("channel doesn't exist");
    }
}



// unknown mode to me