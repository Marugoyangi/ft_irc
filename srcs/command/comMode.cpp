#include "CommandHandler.hpp"

void CommandHandler::handleChannelOperatorMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    std::string channel_name;
    std::string client_name;

    if (params.size() < 3)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }
    client_name = params[2];
    channel_name = params[0];

    //채널 관리자만 invite
    if (!channel.isOperator(client))
    {
        reply(client, "482", channel_name, "You're not channel operator");
        return;
    }

    // 초대할 사용자 확인
    std::map<int, Client> &temp = client.getServer()->getClients();
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

    // 사용자가 채널에 있는지 확인
    if (!channel.isMember(it->first))
    {
        reply(client, "441", client_name, "User not on channel");
        return;
    }
    channel.setOperator(it->second, add);
    // 모드 변경 메시지
    std::string mode_str = add ? "+o" : "-o";
    std::string operator_msg = ":" + client.getNickname() + " MODE " + channel.getChannelName() + \
    " " + mode_str + " " + client_name + "\r\n";
    _reply += operator_msg;
    channel.messageToMembers(client, "MODE " + channel.getChannelName() + " " + mode_str + " " + client_name, "");
}

void CommandHandler::handleChannelMode(Channel &channel, Command const &cmd, Client &client)
{
    std::vector<std::string> params = cmd.getParams();
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    if (!channel.isOperator(client))
    {
        // ERR_CHANOPRIVSNEEDED
        _reply += ":irc.local 482 " + client.getNickname() + " " + channel.getChannelName() + " :You're not channel operator\r\n";
        return;
    }

    std::string modes = params[1];
    bool add = false;
    if (modes[0] == '+')
        add = true;
    else if (modes[0] == '-')
        add = false;
    else
    {
        // ERR_UNKNOWNMODE
        _reply += ":irc.local 472 " + client.getNickname() + " " + modes[0] + " :is unknown mode character\r\n";
        return;
    }
    for (size_t i = 1; i < modes.length(); i++)//
    {
        switch (modes[i])
        {
                case 'o': // 채널 운영자 설정
                    handleChannelOperatorMode(channel, client, params, add);
                    break;
                case 'i':
                    handleChannelInviteMode(channel, client, params, add);
	        	    break;
	        	case 'k':
                    handleChannelKeyMode(channel, client, params, add);
                    break;
                case 'l':
                    handleChannelLimitMode(channel, client, params, add);
                    break;
                case 't':
                    handleChannelTopicMode(channel, client, params, add);
                    break;
                default:
                    _reply += ":irc.local 472 " + client.getNickname() + " " + modes[i] + " :is unknown mode character \r\n";
                    break;
        }
    }
}

void CommandHandler::handleUserMode(Client &client, Command const &cmd)
{
    std::vector<std::string> params = cmd.getParams();
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    std::string modes = params[1];
    bool add = true;

    for (size_t i = 0; i < modes.length(); ++i)
    {
        if (modes[i] == '+')
            add = true;
        else if (modes[i] == '-')
            add = false;
        else
        {
            switch (modes[i])
            {
                case 'i': // Invisible mode
                    client.setInvisibleMode(add);
                    break;
                // 다른 사용자 모드 처리
                default:
                    _reply += ":irc.local 501 " + client.getNickname() + " :Unknown MODE flag\r\n";
                    break;
            }
        }
    }
}


void CommandHandler::mode(Command const &cmd, Client &client, Server &server)
{
    std::vector<std::string> params = cmd.getParams();

    if (params.empty())
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    std::string target = params[0];
    if (target[0] == '#')
    {
        // 채널 모드 처리
        std::map<std::string, Channel> &channels = server.getChannels();
        if (channels.find(target) != channels.end())
        {
            Channel &channel = channels[target];
            handleChannelMode(channel, cmd, client);
        }
        else
        {
            // ERR_NOSUCHCHANNEL
            _reply += ":irc.local 403 " + client.getNickname() + " " + target + " :No such channel\r\n";
        }
    }
    else
    {
        // 사용자 모드 처리
        handleUserMode(client, cmd);
    }
}
