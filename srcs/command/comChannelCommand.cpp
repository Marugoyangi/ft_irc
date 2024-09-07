#include "CommandHandler.hpp"

void CommandHandler::invite(Command &cmd, Client &client, Server &server)
{
    std::string channel_name;
    std::string client_name;
    std::map<std::string, Channel> &channels = server.getChannels();

    std::vector<std::string> _tem = cmd.getParams();
    if (_tem.size() < 2)
    {
        reply(client, "461", "INVITE", "Not enough parameters");
        return;
    }
    client_name = _tem[0];
    channel_name = _tem[1];

    //존재하는 채널인지 확인
    if (channels.find(channel_name) == channels.end())
    {
        reply(client, "403", channel_name, "No such channel");
        return;
    }

    Channel &channel = channels[channel_name];

    //채널 관리자만 invite
    if (!channel.isOperator(client))
    {
        reply(client, "482", channel_name, "You're not channel operator");
        return;
    }

    // 초대할 사용자 확인
    std::map<int, Client> &temp = server.getClients();
    bool nick_found = false;
    std::map<int, Client>::iterator it;
    for (it = temp.begin(); it != temp.end(); it++)
    {
        if (it->second.getNickname() == client_name)
        {
            nick_found = true;
            break;
        }
    }
    if (!nick_found)
    {
        reply(client, "401", client_name, "No such nick");
        return;
    }

    // 사용자가 이미 채널에 있는지 확인
    if (channel.isMember(it->first))
    {
        reply(client, "443", client_name, "User already on channel");
        return;
    }

    // 초대 처리
    channel.addInvitedList(client_name);
    std::string invite_msg = ":" + client.getNickname() + " INVITE " + client_name + " " + channel_name + "\r\n";
    send(it->first, invite_msg.c_str(), invite_msg.length(), 0);
    _reply += ":irc.local 341 " + client.getNickname() + " " + client_name + " " + channel_name + "\r\n";
}

void CommandHandler::topic(Command const &cmd, Client const &client, Server &server)
{
    std::vector<std::string> params = cmd.getParams();
    if (params.size() < 1)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
        return;
    }

    std::string channel_name = params[0];
    std::map<std::string, Channel> &channels = server.getChannels();
    if (channels.find(channel_name) == channels.end())
    {
        // ERR_NOSUCHCHANNEL
        _reply += ":localhost 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n";
        return;
    }
    std::cout << "channel_name: " << channel_name << std::endl;
    Channel &channel = channels[channel_name];
    if (params.size() == 1)
    {
        // 채널 토픽을 요청
        if (channel.getChannelTopic() == "")
        {
            // RPL_NOTOPIC
            _reply += ":localhost 331 " + client.getNickname() + " " + channel_name + " :No topic is set\r\n";
        }
        else
        {
            if (channel.getChannelTopic().size() >= 390)
                _reply += ":localhost 332 " + client.getNickname() + " " + channel_name + " :" + channel.getChannelTopic().substr(0, 390) + "\r\n";
            else
                _reply += ":localhost 332 " + client.getNickname() + " " + channel_name + " :" + channel.getChannelTopic() + "\r\n";
        }
    }
    else
    {
        // 채널 토픽을 설정
        if (!channel.isMember(client.getSocket_fd()))
        {
            // ERR_NOTONCHANNEL
            _reply += ":localhost 442 " + client.getNickname() + " " + channel_name + " :You're not on that channel\r\n";
            return;
        }
        if (!channel.isOperator(client) && channel.isMode(MODE_T))
        {
            // ERR_CHANOPRIVSNEEDED
            _reply += ":localhost 482 " + client.getNickname() + " " + channel_name + " :You're not channel operator\r\n";
            return;
        }
        std::string topic = params[1];
        std::cout << "topic: " << topic << std::endl;
        channel.setChannelTopic(topic);
        channel.setTopicTime(time(NULL));
        // TOPIC 메시지 전송
        std::string mode_msg = ":irc.local TOPIC " + channel.getChannelName() + " " + topic + "\r\n";
        send(client.getSocket_fd(), mode_msg.c_str(), mode_msg.length(), 0);
	    channel.messageToMembers(client, "TOPIC " + channel.getChannelName() + " " + topic, "");
    }
}

void CommandHandler::com332(Client const &client, Channel const channel)
{
    if (channel.getChannelTopic() == "")
        reply(331, channel.getChannelName(), "No topic is set");
    else
    {
        std::stringstream ss;

        ss << channel.getTopicTime();
        _reply += ":irc.local 332 " + client.getNickname() + " " + channel.getChannelName() + " :" + channel.getChannelTopic() + "\r\n";
        _reply += ":irc.local 333 " + client.getNickname() + " " + channel.getChannelName() + " " + client.getSource() + " :" + ss.str() + "\r\n";
    }
}