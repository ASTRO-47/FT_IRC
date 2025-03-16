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
    }
    else if (input == "mode"){
		if (buffer_size < 3){
            if (buffer_size == 2 && channel_exists(_C.get_cmd(1))){
                channelMap[_C.get_cmd(1)].displayModes(_C);
                return;
            }
            send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), client.get_cmd(0)));
			return ;
        }
        mode_handler(client);
    }
    else if (input == "invite"){
        if (!check_user(_C))
            return;
        if (buffer_size < 3 ){
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
    else if (input == "quit")
    {
        handle_quit_cmd(_C);
        return ;
    }
    else if (input != "pong" && input != "PONG" )
    {
        msge = server_prefix + "421 " + _C.get_cmd(0) +  ": unkown command" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
}