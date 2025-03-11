#include "server.hpp"
#include "Channel.hpp"


void    Server::handle_cmd_1(int i)
{
	std::string input = clients[i]->get_cmd(0);
    toLower(input);
    std::cout << input << '\n';
    Client &client = *clients[i];
    size_t buffer_size = clients[i]->get_buffer_size();
    std::string msge;
    clients[i]->parse_command();
    if (check_command(i))
        return ;
	    // join &chan || join #chan not the same thing
    if (input == "join"){ // tolower w compari
        if (buffer_size < 2){
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), clients[i]->get_cmd(0)));
            client.reset();
            return ;
        }
        join_handler(client);
    }
    else if (input == "mode"){
		if (buffer_size < 3){
            // if (buffer_size == 2) ndisplayi lmods dial channel
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), clients[i]->get_cmd(0)));
			return ;
        }
        mode_handler(client);
    }
    // chi user 3ndo smia dial chi channel anfr9 binathom b #
    else if (input == "invite"){
        if (buffer_size < 3 || !check_user(i)){ // fach kansift lrasi katl3 no such nick
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), clients[i]->get_cmd(0)));
            client.reset();
            return;
        }
        std::string invited = client.get_cmd(1);
        std::string chan = client.get_cmd(2);
        invite_user(invited, &client, chan);
    }
        else if (input == "topic")
            topic_handler(client, buffer_size);
        else if (input == "kick")
            kick_handler(client, buffer_size);
    else if (input != "pong" && input != "PONG" )
    {
        msge = server_prefix + "421 " + clients[i]->get_cmd(0) +  ": unkown command" + CRLF;
        send_reply(clients[i]->get_socket_fd(), msge);
    }
}


//vector 
// l9it +o 
// antraiter l arg wnpoppih


// sawb chi function katreturni lik wach dak lmode valid w katseti lik wach khas param wla la


// user cannot promote himself


//vector 
// l9it +o 
// antraiter l arg wnpoppih


// sawb chi function katreturni lik wach dak lmode valid w katseti lik wach khas param wla la


// user cannot promote himself


// check discord diali 7at screen tests dial mode


// join #chan1,#chan1 key key
// pk makhdamach

// check if its a member of the channel before checking if it has a password

