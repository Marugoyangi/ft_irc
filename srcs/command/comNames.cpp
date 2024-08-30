#include "CommandHandler.hpp"

void	CommandHandler::names(Command const &cmd, Client const &client, Server &server)
{
	std::string	channel_name = "";
	std::vector<std::string> _tem = cmd.getParams();

	while (_tem[0].length() > 0)
	{
		if (_tem[0].find(',') != std::string::npos)
		{
			channel_name = _tem[0].substr(0, _tem[0].find(','));
			_tem[0].erase(0, _tem[0].find(',') + 1);
		}
		else
		{
			channel_name = _tem[0];
			_tem[0] = "";
		}
		if (server.getChannels().find(channel_name) != server.getChannels().end())
			com353(server, server.getChannels().at(channel_name));
		com366(client, channel_name);
	}
}

void	CommandHandler::com353(Server &server, Channel const &channel)  //RPL_NAMREPLY
{
	if (channel.isMember(this->_client->getSocket_fd()))
	{
		_reply += ":irc.local 353 " + this->_client->getNickname() + " = " + channel.getChannelName() + " :";
		_reply += channel.getChannelMembers(server) + "\r\n";
	}
}

void	CommandHandler::com366(Client const &client, std::string const &channel_name)  //RPL_ENDOFNAMES
{
	_reply += ":irc.local 366 " + client.getNickname() + " " + channel_name + " :End of /NAMES list.\r\n";
}
