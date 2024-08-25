#include "CommandHandler.hpp"

void	CommandHandler::com001(Client const &client, std::string const &server_name)  //RPL_WELCOME
{
    _reply += ":irc.local 001 " + client.getNickname() + \
                " :Welcome to the " + server_name +\
                " Network, " + client.getNickname() + \
                "!" + client.getUsername() + \
                "@" + client.getHostname() + "\r\n";
}

void CommandHandler::com461(std::string const &nickname, std::string const &cmd)  //ERR_NEEDMOREPARAMS
{
	_reply += ":irc.local 461 " + nickname + " " + cmd + " :Not enough parameters\r\n";
}
