#    SMALL IRC SERVER

https://youtu.be/H9N_l85TpnI --> talking how select, poll and epoll functionx handle multiplexing in the server

struct pollfd {
    int   fd;        // The file descriptor (socket)
    short events;    // Events to monitor (e.g., POLLIN, POLLOUT)
    short revents;   // Events that actually occurred
};

https://www.rfc-editor.org/rfc/rfc2812.html#section-1.1 irc ref

-- the last param in the s(end) sys call: its about how the message gonna sent.(urgnet data, bypass routing, sigpipe.....)

✅ Multiplexing = Monitoring multiple file descriptors (e.g., sockets) at the same time.
✅ Non-Blocking = Instead of waiting for a client to send data, the server checks quickly and moves on if there’s nothing available.

A Simple Analogy
Imagine you're a waiter in a restaurant:

Blocking Mode: You take one order, wait until the food is ready, and then serve it. Meanwhile, other customers are waiting.
Non-Blocking Mode: You check if the food is ready. If not, you move on to serve another table and come back later.
Multiplexing: You monitor multiple tables at once, and when food is ready for any of them, you serve it immediately.
So, in a non-blocking + multiplexed server:

The server does not wait for a single client.
It monitors all clients and only acts when there's data to process.
💡 This is exactly how high-performance servers (like an IRC server) handle thousands of connections efficiently! 🚀

Ports in the range 1–1023 are called privileged ports.