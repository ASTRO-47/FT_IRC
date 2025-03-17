#include "bot.hpp"

int Bot::which_service(std::string& buf){

	if (buf.find("joke") != std::string::npos){
		size_t found = buf.find("joke");
		if (found > 0 && buf[found - 1] != ' ' && buf[found - 1] != ':'){
			return -1;
		}
		size_t end = found + 4;
		if (end < buf.size() && std::isalpha(buf[end])){
			return -1;
		}
		return 1;
	}
	if (buf.find("capital") != std::string::npos){
		size_t found = buf.find("capital");
		if (found > 0 && buf[found - 1] != ' ' && buf[found - 1] != ':'){
			return -1;
		}
		size_t end = found + 7;
		if (std::isalpha(buf[end])){
			return -1;
		}
		buf = buf.substr(end + 1);
		while (!buf.empty() && (buf.back() == '\n' || buf.back() == '\r'))
			buf.pop_back();
		return 2;
	}
	return -1;
}

void Bot::random_joke(std::string &nick){
	srand(time(0));
	int ExtractjokeAt = rand() % 10;
	std::string s = "PRIVMSG " + nick + " :" + _jokes[ExtractjokeAt] + "\r\n";
    send(_socket, s.c_str(), s.length(), 0);
}

void Bot::capital_handler(std::string &buf, std::string &nick){
	std::string country = buf;
	if (_capitals.find(country) != _capitals.end()){
		std::string s = "PRIVMSG " + nick + " :The capital of " + country + " is: " + _capitals[country] + "\r\n";
    	send(_socket, s.c_str(), s.length(), 0);
		return;
	}
	else {
		std::string s = "PRIVMSG " + nick + " :ERROR! try with a different country\r\n";
    	send(_socket, s.c_str(), s.length(), 0);
		return;
	}
}
