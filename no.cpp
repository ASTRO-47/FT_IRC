#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    std::cout << "enter some thing: \n" << std::flush;

    char buffer[100];
    fcntl(0, F_SETFL, O_NONBLOCK);
    int bytes = read(0, buffer, sizeof(buffer));
    if (bytes == -1 && errno == EAGAIN)
        puts("there is nothing to read right now");
    std::cout << "you entered: " << buffer << std::endl;
}

// Summary of Fixes
// Case	Action
// bytes > 0	Process received data.
// bytes == 0	Client closed connection, so close the socket.
// bytes == -1 with EAGAIN/EWOULDBLOCK	No data available, just return (non-blocking mode).
// bytes == -1 with other errors	Log error and close connection.