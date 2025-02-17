#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main() {
    // Create a TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return 1;
    }

    // Bind to port 8080
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Error binding: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        std::cerr << "Error listening: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    // Initialize pollfd array
    std::vector<struct pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0}); // Monitor the server socket

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        // Call poll() to wait for events
        int ready = poll(fds.data(), fds.size(), -1); // Block indefinitely
        if (ready == -1) {
            std::cerr << "Error in poll: " << strerror(errno) << std::endl;
            break;
        }

        // Check events for all file descriptors
        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) 
            {
                if (fds[i].fd == server_fd) 
                {
                    // Accept new connection
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd == -1) 
                    {
                        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
                    } 
                    else 
                    {
                        std::cout << "New client connected: " << client_fd << std::endl;
                        fds.push_back({client_fd, POLLIN, 0}); // Add to poll list
                    }
                }
                else 
                {
                    // Handle data from client
                    char buffer[1024];
                    ssize_t bytes_read = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (bytes_read <= 0) {
                        // Client disconnected
                        std::cout << "Client " << fds[i].fd << " disconnected." << std::endl;
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i); // Remove from poll list
                        --i; // Adjust index after erasing
                    } else {
                        // Process received data
                        buffer[bytes_read] = '\0';
                        std::cout << "Received from " << fds[i].fd << ": " << buffer << std::endl;
                    }
                }
            }

            if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                // Handle errors or hangup
                std::cout << "Error or hangup on fd " << fds[i].fd << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                --i;
            }
        }
    }

    // Cleanup
    for (auto &pfd : fds) close(pfd.fd);
    return 0;
}