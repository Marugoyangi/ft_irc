#include "CommandHandler.hpp"

// void	comJoin(Client const &client, std::vector<std::string> const &param)
void CommandHandler::join(Command &cmd, Client &client, Server &server)
{
	std::string	message = "";
	std::string channel_name = "";
	std::string mode = "";
	size_t comma_pos = 0;
	size_t mode_pos = 0;
	std::vector<std::string> _tem;
	std::map<std::string, Channel> &channels = server.getChannels();

	_tem = cmd.getParams();
	if (_tem.size() < 1)
	{
		reply(461, "JOIN", "Not enough parameters");
		return;
	}
	while ((comma_pos = _tem[0].find(',')) != std::string::npos)
	{
		if (comma_pos > 0)
		{
			channel_name = _tem[0].substr(0, comma_pos);
			_tem[0].erase(1, comma_pos + 1);

			if (_tem.size() > 1)
			{
				mode_pos = _tem[1].find(',');
				if (mode_pos > 0)
				{
					mode = _tem[1].substr(0, mode_pos);
					_tem[1].erase(0, mode_pos + 1);
				}
			}
			if (channels.find(channel_name) != channels.end()) // 기존에 있는 채널일 때
			{
				if (channels[channel_name].addClient(client) == -1)
					continue;
			}
			else
			{
				channels[channel_name] = *(new Channel(channel_name)); // 채널 새로 만듦
				if (channels[channel_name].addClient(client) == -1)
					continue;
			}
			// message = RPL_TOPIC(332)
			message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channel_name + "\r\n";
			message += com353(server, channels[channel_name]);
			message += ":irc.local 366 " + client.getNickname() + " " + channel_name + " :End of /NAMES list.\r\n";
			std::cout << "\033[01m\033[33mmessage to client: " << message << "\033[0m" << std::endl;
			send(client.getSocket_fd(), message.c_str(), message.length(), 0);
			channels[channel_name].messageToMembers(client, "JOIN", channel_name);
		}
	}
	if (_tem[0] != "")
	{
		channel_name = _tem[0];
		if (_tem[1] != "")
			mode = _tem[1];
		if (channels.find(channel_name) != channels.end())
		{
			if (channels[channel_name].addClient(client) == -1)
				return;
		}
		else
		{
			channels[channel_name] = *(new Channel(channel_name));
			if (channels[channel_name].addClient(client) == -1)
				return;
		}
		// message = RPL_TOPIC(332)
		message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channel_name + "\r\n";
		message += com353(server, channels[channel_name]);
		message += ":irc.local 366 " + client.getNickname() + " " + channel_name + " :End of /NAMES list.\r\n";
		std::cout << "\033[01m\033[33mmessage to client: " << message << "\033[0m" << std::endl;
		send(client.getSocket_fd(), message.c_str(), message.length(), 0);
		channels[channel_name].messageToMembers(client, "JOIN", channel_name);
	}
	return;
}

std::string CommandHandler::com461(Client const &client, Command const &cmd)
{
	Command _cmd;
	std::vector<std::string> _tem;

	_tem.push_back(client.getUsername());
	_tem.push_back(cmd.getCommand());
	_tem.push_back("Not enough parameters");
	_cmd.setCommand("", client.getUsername(), "461", _tem);
	return (_cmd.deparseCommand());
}

// std::string	com332(Client const &client, Command const &cmd)
// {
// }

// std::string	com333(Client const &client, Command const &cmd)
// {
// }

std::string	CommandHandler::com353(Server &server, Channel const &channel)
{
	std::string ret;

	ret = ":irc.local 353 " + this->_client->getNickname() + " = " + channel.getChannelName() + " :";
	ret += channel.getChannelMembers(server) + "\r\n";
	return (ret);
}

// std::string	com366(Client const &client, Command const &cmd)
// {
// }

void CommandHandler::part(Command &cmd, Client &client, std::map<std::string, Channel> &channels)
{
	size_t comma_pos;
	std::string channel_name;
	std::string message;
	std::vector<std::string> _tem = cmd.getParams();

	if (_tem.size() < 1)
	{
		reply(461, "PART", "Not enough parameters");
		return;
	}
	while ((comma_pos = _tem[0].find(',')) != std::string::npos)
	{
		if (comma_pos > 0)
		{
			channel_name = _tem[0].substr(0, comma_pos);
			_tem[0].erase(1, comma_pos + 1);
		}
		if (channels.find(channel_name) == channels.end())
		{
			reply(403, channel_name, "No such channel");
			return;
		}
		if (!channels[channel_name].isMember(client.getSocket_fd()))
		{
			reply(442, channel_name, "You're not on that channel");
			return;
		}
		message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PART :" + channel_name;
		channels[channel_name].messageToMembers(client, "PART", channel_name);
		channels[channel_name].removeClient(client.getSocket_fd());
	}
	if (_tem[0] != "")
	{
		channel_name = _tem[0];
		if (channels.find(channel_name) == channels.end())
		{
			reply(403, channel_name, "No such channel");
			return;
		}
		if (!channels[channel_name].isMember(client.getSocket_fd()))
		{
			reply(442, channel_name, "You're not on that channel");
			return;
		}
	}
	message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PART :" + channel_name +"\r\n";
	std::cout << "\033[01m\033[33mmessage to client: " << message << "\033[0m" << std::endl;
	send(client.getSocket_fd(), message.c_str(), message.length(), 0);
	
	channels[channel_name].messageToMembers(client, "PART", channel_name);
	channels[channel_name].removeClient(client.getSocket_fd());
}