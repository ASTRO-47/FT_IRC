#include "server.hpp"
#include "Channel.hpp"

void Server::topic_handler(Client &client, size_t buffer_size){
    if (buffer_size < 2)
        send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), client.get_cmd(0)));
    else if (buffer_size == 2){
        std::string name = client.get_cmd(1);
        std::string msg = channelMap[name].getTopicString();
        if (msg.empty()){
            send_reply(client.get_socket_fd(), RPL_NOTOPIC(client.get_nick_name(), name));
            return;
        }
        channelMap[name].broadcastToAllMembers(RPL_TOPIC(client.get_nick_name(), name, msg));
        return;
    }
    std::vector<std::string> vec = client.get_cmd_buffer();
    std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), ":");
    if (it != vec.end()){
        it++;
    std::string name = client.get_cmd(1);
    while (it != vec.end()){
        channelMap[name].getTopicString() += *it + ' ';
        it++;
    }
    std::string topic = channelMap[client.get_cmd(1)].getTopicString();
    std::string btopic = ":" + topic;
    channelMap[client.get_cmd(1)].setTopic(true);
    channelMap[client.get_cmd(1)].setTopicString(topic);
    channelMap[client.get_cmd(1)].broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channelMap[client.get_cmd(1)].getChannelName(), client.get_ip(), client.get_hostname(), btopic , "TOPIC"));
    return;
    }
    size_t pos = client.get_second_buffer().find(':');
    if (pos != std::string::npos){
        std::string topic = client.get_second_buffer().substr(pos + 1, client.get_second_buffer().length() - pos - 2); // tatbdl hadchi
        std::string btopic = ":" + topic;
        channelMap[client.get_cmd(1)].setTopic(true);
        channelMap[client.get_cmd(1)].setTopicString(topic);
        channelMap[client.get_cmd(1)].broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channelMap[client.get_cmd(1)].getChannelName(), client.get_ip(), client.get_hostname(), btopic , "TOPIC"));
    }
    else{
        std::string topic = client.get_cmd(2);
        std::string btopic = ":" + client.get_cmd(2);
        channelMap[client.get_cmd(1)].setTopic(true);
        channelMap[client.get_cmd(1)].setTopicString(client.get_cmd(2));
        channelMap[client.get_cmd(1)].broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channelMap[client.get_cmd(1)].getChannelName(), client.get_ip(), client.get_hostname(), btopic , "TOPIC"));
    }
}