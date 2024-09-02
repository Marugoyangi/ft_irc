#include "CommandHandler.hpp"

void	CommandHandler::topic(Command const &cmd, Client const &client, Server &server)
{
	std::string	channel_name = "";
	std::string topic = "";
	std::vector<std::string> _tem = cmd.getParams();
	std::map<std::string, Channel*> &channels = server.getChannels();

	if (_tem.size() < 1)
	{
		com461(client.getNickname(), "TOPIC");
		return;
	}
	channel_name = _tem[0];

	if (channels.find(channel_name) == channels.end())   
	{
		reply(403, channel_name, "No such channel");
		return;
	}

	if (_tem.size() == 1)											// topic 조회.
	{
		topic = channels[channel_name]->getChannelTopic();
		if (topic == "")
			reply(331, channel_name, "No topic is set");
		else
			com332(client, channels[channel_name]);
		return;
	}
	else															// topic 변경, 권한 설정 필요.
	{
		if (!channels[channel_name]->isMember(client.getSocket_fd()))
		{
			reply(442, channel_name, "You're not on that channel");
			return ;
		}
		for (size_t i = 1; i < _tem.size(); i++)
			topic += _tem[i];
		channels[channel_name]->setChannelTopic(topic);
		_reply += client.getSource() + " TOPIC " + channel_name + " :" + topic + "\r\n";
		channels[channel_name]->messageToMembers(client, "TOPIC " + channel_name, topic);
	}
}

void	CommandHandler::com332(Client const &client, Channel const *channel)  //RPL_TOPIC  토픽 조회
{
	if (channel->getChannelTopic() == "")
		reply(331, channel->getChannelName(), "No topic is set");
	else
	{
		std::stringstream ss;

		ss << channel->getTopicTime();
		_reply += ":irc.local 332 " + client.getNickname() + " " + channel->getChannelName() + " :" + channel->getChannelTopic() + "\r\n";
		_reply += ":irc.local 333 " + client.getNickname() + " " + channel->getChannelName() + " " + client.getSource() + " :" + ss.str() + "\r\n";
	}
}