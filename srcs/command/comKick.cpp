#include "CommandHandler.hpp"

void	CommandHandler::kick(Command &cmd, Client &client, Server &server)
{
	size_t	comma = 0;
	std::string	channel_name = "";
	std::string	user_name = "";
	int	target_fd;
	std::string	message = "";
	std::vector<std::string> tem = cmd.getParams();
	std::map<std::string, Channel> &channels = server.getChannels();
	std::map<int, Client> &clients = server.getClients();
	std::map<int, Client>::iterator it;

	if (tem.size() < 2)
	{
		_reply += ":irc.local 461 " + client.getNickname() + " KICK : Not enough parameters\r\n";
		return ; 
	}
	if (tem.size() > 2)
		message = tem[2];
	channel_name = tem[0];
	while (tem[1].length() > 0)
	{		
		if ((comma= tem[1].find(',')) != std::string::npos)
		{
			user_name = tem[1].substr(0, comma);
			tem[1].erase(0, comma + 1);
		}
		else
		{
			user_name = tem[1];
			tem[1] = "";
		}

		//KICK 할 유저의 fd 구하기, 없으면 -1
		target_fd = -1;
		for (it = clients.begin() ; it != clients.end(); it++)
		{
			if (it->second.getNickname() == user_name)
				target_fd = it->first;
		}

		if (channels.find(channel_name) == channels.end())
		{
    		_reply += ":irc.local 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n";
			continue;
		}
		else if (!channels[channel_name].isMember(client.getSocket_fd()))
		{
			_reply += ":irc.local 442 " + client.getNickname() + " " + channel_name + " You're not on that channel!\r\n";
			continue;
		}
		else if (target_fd == -1)
		{
			_reply += "irc.local 401 " + client.getNickname() + " " + user_name + " :No such nick\r\n";
			continue;
		}
		else if (!channels.at(channel_name).isMember(target_fd))
		{
			_reply += "irc.local 441 " + client.getNickname() + " " + user_name + +" " + channel_name + " :They are not on that channel\r\n";
			continue;
		}
		else if (!channels.at(channel_name).isOperator(client))
		{
			_reply += "irc.local 482 " + client.getNickname() + " " + channel_name + " :You must be a channel op or higher to kick a more privileged user.\r\n";
			continue;
		}
		channels.at(channel_name).messageToMembersIncludeSelf(client, "KICK " + channel_name + " " + user_name, message);
		channels.at(channel_name).removeClient(target_fd);
	}
}
