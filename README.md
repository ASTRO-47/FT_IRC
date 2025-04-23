ft_irc

ft_irc is a fully functional IRC (Internet Relay Chat) server implementation created for the 1337 School ft_irc project.

📖 Table of Contents

Overview

Features

Architecture

Installation

Usage

Supported Commands

Contributing

License

📝 Overview

This project implements the IRC protocol (RFC 1459) in C++98, allowing multiple clients to connect, join channels, send messages, and manage channel modes. It demonstrates network programming, concurrency, command parsing, and adherence to protocol specifications.

⚙️ Features

TCP-based server supporting multiple simultaneous clients

Channel creation and management

Private messages and channel broadcasts

Channel modes: +o (operator), +i (invite-only), +t (topic settable by operators)

Invitation and kick functionality

Topic command for channels

Graceful handling of invalid commands and errors

🏗️ Architecture

Server: Listens on a TCP port, accepts new client connections, and manages client sessions.

Client Manager: Tracks connected clients, their nicknames, and current channels.

Command Parser: Parses incoming messages, validates commands, and dispatches handlers.

Channel Manager: Maintains channels, their modes, topics, and member lists.

Event Loop: Uses select() for I/O multiplexing to handle multiple sockets in a single-threaded loop.

📦 Installation

Clone the repository

git clone https://github.com/youruser/ft_irc.git
cd ft_irc

Compile

make

🚀 Usage

Start the server

./ircserv <PORT> <PASSWORD>

Connect with an IRC client (e.g., Weechat, irssi, or nc)

nc -c 127.0.0.1 <PORT>

📜 Supported Commands

Command

Description

PASS <password>

Set connection password

NICK <nickname>

Set or change nickname

USER <user> 0 * :<realname>

Set user information

JOIN <channel>

Join or create a channel

PART <channel>

Leave a channel

PRIVMSG <target> :<message>

Send a private or channel message

NOTICE <target> :<message>

Send a notice

MODE <channel> +/-m <modes>

Change channel modes

TOPIC <channel> :<topic>

Set or view channel topic

INVITE <nick> <channel>

Invite user to channel

KICK <channel> <nick> :<reason>

Kick user from channel
