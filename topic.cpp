#include "server.hpp"
#include "Channel.hpp"

void Server::topic_handler(Client &client, size_t buffer_size){
        if (buffer_size < 2) // needmoreparams
        return;
    else if (buffer_size == 2){
        std::string name = client.get_cmd(1);
        std::string msg = channelMap[name]->getTopic();
        if (msg.empty()){
            puts("no topic set");
            return;
        }
        send(client.get_socket_fd(), msg.c_str(), msg.length(), 0);
        return;
    }
    std::vector<std::string> vec = client.get_cmd_buffer();
    std::vector<std::string>::iterator it = std::find(vec.begin(), vec.end(), ":");
    if (it != vec.end()){
        it++;
    std::string name = client.get_cmd(1);
    while (it != vec.end()){
        channelMap[name]->getTopic() + *it + ' ';
        it++;
    }
    std::string topic = channelMap[client.get_cmd(1)]->getTopic();
    topic[topic.length() - 1] = '\0';
    channelMap[client.get_cmd(1)]->setTopic(topic);
    std::cout << "topic set" << '\n';
    return;
    }
    size_t pos = client.get_buffer().find(':');
    if (pos != std::string::npos){
        std::string topic = client.get_buffer().substr(pos + 1, client.get_buffer().length() - pos - 2); // tatbdl hadchi
        std::cout << "topic is " << topic << '\n';
		// khs topic ndiro lfo9 bach fga3 l7alat nreturnih
    }
    else
        std::string topic = client.get_cmd(2);
}