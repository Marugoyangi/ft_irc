#include "CommandHandler.hpp"

// void	comJoin(Client const &client, std::vector<std::string> const &param)
void CommandHandler::join(Command &cmd, Client &client, Server &server)
{
	std::string channel_name = "";
	std::string key = "";
	size_t comma_pos = 0;
	size_t key_pos = 0;
	std::vector<std::string> _tem;
	std::map<std::string, Channel> &channels = server.getChannels();

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
			_tem[0].erase(0, comma_pos + 1);
		}
		else
		{
			channel_name = _tem[0];
			_tem[0] = "";
		}

		if (channel_name[0] != '#')
		{
			reply(476, client.getNickname(), std::string(channel_name + "Invalid channel name"));
			continue;
		}

		if (_tem.size() > 1 && _tem[1].length() > 0)
		{
			if ((key_pos = _tem[1].find(',')) != std::string::npos)
			{
				key = _tem[1].substr(0, key_pos);
				_tem[1].erase(0, key_pos + 1);
			}
			else
			{
				key = _tem[1];
				_tem[1] = "";
			}
		}

		if (channels.find(channel_name) != channels.end()) // 기존에 있는 채널일 때
		{
			if (channels[channel_name].isMode(MODE_L) && \
			((int)channels[channel_name].getFdList().size() >= channels[channel_name].getLimit())) // 채널 인원 제한
			{
				std::cout << "channel limit" << std::endl;
				std::cout << channels[channel_name].getChannelMembers(channels[channel_name], server) << std::endl;
				std::string tmp = client.getNickname() + " " + channel_name;
				reply(471, tmp, "Cannot join channel (+l)");
				continue;
			}
			else if (channels[channel_name].isMode(MODE_I) && !channels[channel_name].checkInvitedList(client)) // 초대만 가능한 채널
			{
				std::string tmp = client.getNickname() + " " + channel_name;
				reply(473, tmp, "Cannot join channel (+i)");
				continue;
			}
			else if (channels[channel_name].isMode(MODE_K) && channels[channel_name].getKey() != key) // 암호 확인
			{
				std::string tmp = client.getNickname() + " " + channel_name;
				reply(475, tmp,"Cannot join channel (+k)");
				continue;
			}
			if (channels[channel_name].addClient(client) == -1)
				continue;
		}
		else
		{
			channels.insert(std::pair<std::string, Channel>(channel_name, Channel(channel_name)));
			if (key != "")
			{
				channels[channel_name].setKey(key);
				channels[channel_name].setMode(MODE_K);
			}
			channels[channel_name].addClient(client);
			channels[channel_name].setOperator(client, true); // 채널 최초 생성시 관리자 지정
			std::string operator_msg = ":irc.local MODE " + channel_name + " +o " + client.getNickname() + "\r\n";
			send(client.getSocket_fd(), operator_msg.c_str(), operator_msg.length(), 0);
		}
		if (channels.find(channel_name) != channels.end() && channels[channel_name].getChannelTopic() != "")
			com332(client, channels[channel_name]);
		_reply += client.getSource() + " JOIN :" + channel_name + "\r\n";
		com353(server, channels[channel_name]);
		com366(client, channel_name);
		channels[channel_name].messageToMembers(client, "JOIN", channel_name);
	}
	return;
}
