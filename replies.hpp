#include "server.hpp"

#define sprefix ":ft_irc_1337"

#define ERR_INVITEONLYCHAN(target, channel) (std::string(sprefix) + " 473 " + target + " " + channel + " :Cannot join channel (+i)\r\n")

#define ERR_BADCHANNELKEY(target, channel) (std::string(sprefix) + " 475 " + target + " " + channel + " :Cannot join channel (+k)\r\n")

#define ERR_NEEDMOREPARAMS(target, command) (std::string(sprefix) + " 461 " + target + " " + command + " :Not enough parameters\r\n")

#define ERR_CHANOPRIVSNEEDED(target, channel) (std::string(sprefix) + " 482 " + target + " " + channel + " :You're not channel operator\r\n")

#define ERR_NOSUCHCHANNEL(channel) (std::string(sprefix) + " 403 " + channel + " :No such channel\r\n")