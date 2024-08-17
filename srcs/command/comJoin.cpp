#include "../client/Client.hpp"
#include "../server/Server.hpp"
#include "Command.hpp"

void	comJoin(Client client, Command const &cmd)
{
	std::string message;

	if (client.isRegist() == false)
	{
		message = "451 : You are not registered\r\n";
		send(client.getFd(), message.c_str(), message.length(), 0);
	}
	else
	{
		message = ":irc.local NOTICE * :*** You have joined the channel\r\n";
		send(client.getFd(), message.c_str(), message.length(), 0);
	}
}