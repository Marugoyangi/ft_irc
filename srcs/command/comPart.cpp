#include "CommandHandler.hpp"

void CommandHandler::part(Command &cmd, Client &client, std::map<std::string, Channel> &channels)
{
	size_t comma_pos = 0;
	std::string channel_name = "";
	std::vector<std::string> _tem = cmd.getParams();

	if (_tem.size() < 1)
	{
		_reply += ":irc.local 461 " + client.getNickname() + " PART : Not enough parameters\r\n";
		return;
	}

	while (_tem[0].length() > 0)
	{
		if ((comma_pos = _tem[0].find(',')) != std::string::npos)
		{
			channel_name = _tem[0].substr(0, comma_pos);
			std::cout << _tem[0] << " ";
			_tem[0].erase(0, comma_pos + 1);
			std::cout << _tem[0] << std::endl;
		}
		else
		{
			channel_name = _tem[0];
			_tem[0] = "";
		}
		if (channels.find(channel_name) == channels.end())
		{
			reply(403, channel_name, "No such channel");
			continue;
		}
		if (!channels[channel_name].isMember(client.getSocket_fd()))
		{
			reply(442, channel_name, "You're not on that channel");
			continue;
		}
		_reply += client.getSource() + " PART :" + channel_name + "\r\n";
		channels[channel_name].messageToMembers(client, "PART", channel_name);
		channels[channel_name].removeClient(client.getSocket_fd());
		channels[channel_name].removeOperator(client.getNickname());
	}
}
