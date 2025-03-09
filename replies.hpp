#include "server.hpp"


#define prefix ":ft_irc_1337"

#define ERR_INVITEONLYCHAN(target, channel) (std::string(prefix) + " 473 " + target + " " + channel + " :Cannot join channel (+i)\r\n")

#define ERR_BADCHANNELKEY(target, channel) (std::string(prefix) + " 475 " + target + " " + channel + " :Cannot join channel (+k)\r\n")

#define ERR_NEEDMOREPARAMS(target, command) (std::string(prefix) + " 461 " + target + " " + command + " :Not enough parameters\r\n")