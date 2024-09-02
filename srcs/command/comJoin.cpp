#include "CommandHandler.hpp"

// void	comJoin(Client const &client, std::vector<std::string> const &param)
void CommandHandler::join(Command &cmd, Client &client, Server &server)
{
	std::string channel_name = "";
	std::string mode = "";
	size_t comma_pos = 0;
	size_t mode_pos = 0;
	std::vector<std::string> _tem;
	std::map<std::string, Channel*> &channels = server.getChannels();

	_tem = cmd.getParams();
	if (_tem.size() < 1)
	{
		com461(client.getNickname(), "JOIN");
		return;
	}
	while (_tem[0].length() > 0)
	{
		if ((comma_pos = _tem[0].find(',')) != std::string::npos)
		{
			channel_name = _tem[0].substr(0, comma_pos);
			_tem[0].erase(1, comma_pos + 1);
		}
		else
		{
			channel_name = _tem[0];
			_tem[0] = "";
		}
		if (channel_name[0] != '#')
		{
			_reply += ":localhost 476 " + client.getNickname() + " " + channel_name + " :Invalid channel name\r\n";
			return;
		}

		// mode처리 필요함/////////////////////////////////////
		// mode처리 필요함/////////////////////////////////////
		if (_tem.size() > 1)
			{
				mode_pos = _tem[1].find(',');
				if (mode_pos > 0)
				{
					mode = _tem[1].substr(0, mode_pos);
					_tem[1].erase(0, mode_pos + 1);
				}
			}
		// mode처리 필요함/////////////////////////////////////
		// mode처리 필요함/////////////////////////////////////

		if (channels.find(channel_name) != channels.end()) // 기존에 있는 채널일 때
		{
			if (channels[channel_name]->addClient(client) == -1)
				continue;
		}
		else
		{
			Channel	*tem = new Channel(channel_name);
			channels.insert(std::make_pair(channel_name, tem)); // 채널 새로 만듦
			if (channels[channel_name]->addClient(client) == -1)
				continue;
		}
		if (channels.find(channel_name) != channels.end() && channels[channel_name]->getChannelTopic() != "")
			com332(client, channels[channel_name]);
		_reply += client.getSource() + " JOIN :" + channel_name + "\r\n";
		com353(server, channels[channel_name]);
		com366(client, channel_name);
		channels[channel_name]->messageToMembers(client, "JOIN", channel_name);
	}
	return;
}
