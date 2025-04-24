# ft_irc

> A modern C++98 IRC server built for the 1337 School **ft_irc** project

---

## 📖 Table of Contents
1. [Project Overview](#project-overview)
2. [Key Features](#key-features)
3. [High-Level Architecture](#high-level-architecture)
4. [Quick Start](#quick-start)
   - [Prerequisites](#prerequisites)
   - [Installation](#installation)
   - [Running the Server](#running-the-server)
5. [Usage Examples](#usage-examples)
6. [Supported IRC Commands](#supported-irc-commands)

---

## Project Overview

**ft_irc** is a single-threaded, event-driven IRC (Internet Relay Chat) server written in C++98, fully compliant with RFC 1459. It demonstrates low-level socket programming, I/O multiplexing with `poll()`, and robust command parsing.


## Key Features

- **Multi-client support** using `poll()` for non-blocking I/O
- **Channel management**: create, join, part, kick, invite
- **User modes & channel modes**: operators (+o), invite-only (+i), topic control (+t)
- **Private messaging** (`PRIVMSG`) and **broadcasts**
- **Topic management**: view and set with `TOPIC` command
- Graceful error handling and protocol compliance


## High-Level Architecture

```
+----------------+        +-------------+       +-----------------+
| Client Sockets | <-->   | Event Loop  | <---> | Command Handlers|
+----------------+        +-------------+       +-----------------+
                              ^   ^   ^
                              |   |   |
                         +----+   |   +----+
                         |        |        |
                    +----------+   |   +----------+
                    | Channel  |   |   | Client   |
                    | Manager  |   |   | Manager  |
                    +----------+   |   +----------+
                                  |
                             +----------+
                             |  Logger  |
                             +----------+
```

- **Event Loop**: uses `poll()` to watch all client sockets.
- **Client Manager**: tracks nicknames, registrations, and channels.
- **Channel Manager**: maintains channel state, modes, topics, and member lists.
- **Command Handlers**: parse and respond to IRC commands.
- **Logger**: optional logging of server events.


## Quick Start

### Prerequisites
- A POSIX-compliant OS (Linux/macOS)
- A C++98-compatible compiler (e.g., `g++`)

### Installation
```bash
$ git clone https://github.com/ASTRO-47/FT_IRC.git
$ cd ft_irc
$ make
```

### Running the Server
```bash
$ ./ircserv <PORT> <PASSWORD>
```
- **PORT**: TCP port to listen on (e.g., `6667`)
- **PASSWORD**: server password for clients


## Usage Examples

1. **Connect via `nc`**:
   ```bash
   $ nc localhost 6667
   PASS mypass
   NICK Alice
   USER alice 0 * :Alice Example
   JOIN #general
   PRIVMSG #general :Hello, IRC!
   ```

2. **Using an IRC client** (e.g., `irssi`):
   ```bash
   /server localhost 6667
   /pass mypass
   /nick Alice
   /join #general
   /msg #general Hello from irssi!
   ```


## Supported IRC Commands

| Command                          | Description                       |
| -------------------------------- | --------------------------------- |
| `PASS <password>`                | Authenticate with server password |
| `NICK <nickname>`                | Set or change your nickname       |
| `USER <user> 0 * :<realname>`    | Register user details             |
| `JOIN <#channel>`                | Join or create a channel          |
| `PRIVMSG <target> :<message>`    | Send private or channel message   |
| `MODE <channel> +/-m <modes>`    | Change channel modes              |
| `TOPIC <#channel> :<topic>`      | View or set channel topic         |
| `INVITE <nick> <#channel>`       | Invite a user to a channel        |
| `KICK <#channel> <nick> :<reason>` | Remove a user from a channel     |

*Happy chatting!*

