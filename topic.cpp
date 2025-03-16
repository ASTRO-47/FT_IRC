#include "server.hpp"
#include "Channel.hpp"

void Server::topic_handler(Client &client, size_t buffer_size){
    if (buffer_size < 2){
        send_reply(client.get_socket_fd(), ERR_NEEDMOREPARAMS(client.get_nick_name(), client.get_cmd(0)));
        return;
    }
    std::string chan = client.get_cmd(1);
    if (!channel_exists(chan)){
        send_reply(client.get_socket_fd(), ERR_NOSUCHCHANNEL(chan));
        return;
    }
    Channel &channel = channelMap[chan];
    if (buffer_size == 2 || (buffer_size == 3 && client.get_cmd(2) == ":")){
        std::string msg = channel.getTopicString();
        if (msg.empty())
            send_reply(client.get_socket_fd(), RPL_NOTOPIC(client.get_nick_name(), chan));
        else
            send_reply(client.get_socket_fd(), RPL_TOPIC(client.get_nick_name(), chan, msg));
        return;
    }
    if (channel.getTopicFlag() && !channel.isOperator(&client)){
        send_reply(client.get_socket_fd(), ERR_CHANOPRIVSNEEDED(client.get_nick_name(), chan));
        return;
    }
    std::vector<std::string> vec = client.get_cmd_buffer();
    std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), ":");
    std::string newTopic;
    if (it != vec.end()){
        it++;
    while (it != vec.end()){
        newTopic += *it + ' ';
        it++;
    }
    std::string btopic = ":" + newTopic;
    channel.setTopic(true);
    channel.setTopicString(newTopic);
    Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), btopic , "TOPIC"), channel);
    return;
    }
    size_t pos = client.get_second_buffer().find(':');
    if (pos != std::string::npos){
        std::string topic = client.get_second_buffer().substr(pos + 1, client.get_second_buffer().length() - pos - 2);
        std::string btopic = ":" + topic;
        channel.setTopic(true);
        channel.setTopicString(topic);
        Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), btopic , "TOPIC"), channel);
    }
    else{
        std::string topic = client.get_cmd(2);
        std::string btopic = ":" + client.get_cmd(2);
        channel.setTopic(true);
        channel.setTopicString(client.get_cmd(2));
        Channel::broadcastToAllMembers(OPER_SUCCESS(client.get_nick_name(), channel.getChannelName(), client.get_ip(), client.get_user_name(), btopic , "TOPIC"), channel);
    }
}