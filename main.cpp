#include "server.hpp"

int main() 
{
    // Step 1: Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Step 2: Bind the socket to an IP address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6667); // IRC default port
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket\n";
        close(serverSocket);
        return 1;
    }

    // Step 3: Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port 6667...\n";

    // Step 4: Accept a client connection
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == -1) {
        std::cerr << "Failed to accept client connection\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected!\n";

    // Step 5: Communicate with the client
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Client disconnected\n";
            break;
        }
        std::cout << "Received: " << buffer << std::endl;
        send(clientSocket, buffer, bytesReceived, 0); // Echo back the message
    }

    // Step 6: Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}