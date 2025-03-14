#include "server.hpp"

#define sprefix ":ircserv"

#define ERR_INVITEONLYCHAN(target, channel) (std::string(sprefix) + " 473 " + target + " " + channel + " :Cannot join channel (+i)\r\n")

#define ERR_BADCHANNELKEY(target, channel) (std::string(sprefix) + " 475 " + target + " " + channel + " :Cannot join channel (+k)\r\n")

#define ERR_NEEDMOREPARAMS(target, command) (std::string(sprefix) + " 461 " + target + " " + command + " :Not enough parameters\r\n")

#define ERR_CHANOPRIVSNEEDED(target, channel) (std::string(sprefix) + " 482 " + target + " " + channel + " :You're not channel operator\r\n")

#define ERR_NOSUCHCHANNEL(channel) (std::string(sprefix) + " 403 " + channel + " :No such channel\r\n")

#define CHANNEL_QUIT(target, channel, ip, username) (":" + target + "!~" + username + "@" + ip + " QUIT :" + channel + "\r\n")

#define CHANNEL_JOIN(target, channel, ip, hostname) (":" + target + "!~" + hostname + "@" + ip + " JOIN " + channel + "\r\n")

#define CHANNEL_MODES(target, channel, ip, hostname, modes) (":" + target + "!~" + hostname + "@" + ip + " MODE " + channel + " +t\r\n")

#define RPL_NAMREPLY(target, channel, members) (std::string(sprefix) + " 353 " + target + " @ " + channel + " :" + members + "\r\n")


#define RPL_ENDOFNAMES(target, channel) (std::string(sprefix) + " 366 " + target + " " + channel + " :End of /NAMES list.\r\n")

#define OPER_SUCCESS(target, channel, ip, username, oper, command) (":" + target + "!~" + username + "@" + ip + " " + command  +  " " +  channel + " " + oper + " \r\n")

#define KICK_SUCCESS(target, channel, ip, username, kicked, reason, command) (":" + target + "!~" + username + "@" + ip + " " + command  +  " " +  channel + " " + kicked + " :" + reason + "\r\n")


#define RPL_NOTOPIC(target, channel) (std::string(sprefix) + " 331 " + target + " " + channel + " :No topic is set.\r\n")


#define RPL_TOPIC(target ,channel, topic) (std::string(sprefix) + " 332 " + target + " " + channel + topic + " \r\n")

#define ERR_UNKNOWNMODE(target, oper) (std::string(sprefix) + " 472 " + target + oper + " :is an unknown mode char to me\r\n")

#define ERR_NOSUCHNICK(target, nick) (std::string(sprefix) + " 401 " + target + " " + nick + " :No such nick/channel\r\n")

#define ERR_USERONCHANNEL(user, channel, target) (std::string(sprefix) + " 443 " + target + " " + user + " " + channel + " :is already on channel\r\n")

#define ERR_CHANNELISFULL(channel, target) (std::string(sprefix) + " 471 " + target + " " + channel + " :Cannot join channel, Channel is full (+l)\r\n")

#define ERR_USERNOTINCHANNEL(channel, target, tobekicked)(std::string(sprefix) + " 441 " + target + " " + tobekicked + " " + channel + " :They aren't on that channel\r\n")

#define ERR_NOTONCHANNEL(channel, target) (std::string(sprefix) + " 442 " + target + " " + channel + " :You're not on that channel\r\n")


invite #chan a
:ft_irc.1337.ma 403 m a :No such channel

checkiha dbbbb

