#include "server.hpp"

Server::Server(){}

// bool check_numeric(std::string m)
// {
//     for (size_t i = 0; i , m.length();i++)
//     {
//         if (!std::isdigit(m[i]))
//             return false;
//     }
//     return true;
// }

void Server::server_setup(std::string _port)
{
    char *checker = NULL;
    port = std::strtod(_port.c_str(), &checker);
    if (checker[0] != '\0')
        throw std::runtime_error("invalid port format");

    // need to check the port chosen

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
        throw std::runtime_error("failed to create socket");
    sock_addr.sin_family = AF_INET; //  select the ipv4 protocols
    sock_addr.sin_addr.s_addr = INADDR_ANY; // chose the network interfaces will listen on
    sock_addr.sin_port = htons(port); // the port will listen on
    if (bind(server_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        close (server_socket);
        throw std::runtime_error("bind failed");
    }
    if (listen(server_socket, 5) < 0)
    {
        throw std::runtime_error("listen failed");
        close (server_socket);
    }
    std::cout << "server listening on port: " << port << std::endl;
}

void Server::wait_connections()
{
    while (true){
        sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        // protect accept
        std::cout << "Client connected\n";
        char buffer[1024];
        while (true){
            int bytes = recv(client_socket, buffer, 1024, 0);
            if (bytes <= 0){
                std::cout << "client disconnected\n";
                break;
            }
            buffer[bytes - 1] = '\0';
            std::cout << "recieved: " << buffer << std::endl;
            send(client_socket, "hello from astro server\n", 25, 0);
        }
        close (client_socket);
    }
}


Server::~Server()
{
    close(server_socket);
    close(client_socket);
}