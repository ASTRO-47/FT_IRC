#include "server.hpp"
#include "Channel.hpp"

void    Server::handle_cmd_1(Client &_C)
{
    _C.parse_command();
    if (!_C.get_buffer_size())
        return ;
	std::string input = _C.get_cmd(0);
    toLower(input);
    Client &client = _C;
    size_t buffer_size = _C.get_buffer_size();
    std::string msge;
    if (check_command(_C))
        return ;
    if (input == "join"){
        if (buffer_size < 2){
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), _C.get_cmd(0)));
            client.reset();
            return ;
        }
        join_handler(client);
        std::cout << "name : " << channelMap["#chan"].getChannelName() << '\n';
    }
    else if (input == "mode"){
		if (buffer_size < 3){
            if (buffer_size == 2 && channel_exists(client.get_cmd(1)))
                send_reply(client.get_socket_fd(), CHANNEL_MODES(client.get_nick_name(), client.get_cmd(1), client.get_ip(), client.get_user_name()));
            else
                send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), client.get_cmd(0)));
			return ;
        }
        mode_handler(client);
    }
    else if (input == "invite"){
        if (buffer_size < 3 || !check_user(_C)){ // fach kansift lrasi katl3 no such nick
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), _C.get_cmd(0)));
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
        msge = server_prefix + "421 " + _C.get_cmd(0) +  ": unkown command" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
}



// user cannot promote himself


// check discord diali 7at screen tests dial mode


// join #chan1,#chan1 key key
// pk makhdamach

// check if its a member of the channel before checking if it has a password

