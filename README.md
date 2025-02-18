#    SMALL IRC SERVER

https://youtu.be/H9N_l85TpnI --> talking how select, poll and epoll functionx handle multiplexing in the server

struct pollfd {
    int   fd;        // The file descriptor (socket)
    short events;    // Events to monitor (e.g., POLLIN, POLLOUT)
    short revents;   // Events that actually occurred
};

https://www.rfc-editor.org/rfc/rfc2812.html#section-1.1 irc ref