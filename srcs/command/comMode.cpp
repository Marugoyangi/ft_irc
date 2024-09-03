#include "CommandHandler.hpp"

void CommandHandler::handleChannelOperatorMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if (params.size() < 3)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    std::string target_nick = params[2];
    Client *target_client = channel.getClient(target_nick);
    if (!target_client)
    {
        // ERR_USERNOTINCHANNEL
        _reply += ":localhost 441 " + client.getNickname() + " " + target_nick + " " + channel.getChannelName() + " :They aren't on that channel\r\n";
        return;
    }

    channel.setOperator(*target_client, add);

    // 모드 변경 메시지
    std::string mode_str = add ? "+o" : "-o";
    std::string mode_msg = ":" + client.getNickname() + " MODE " + channel.getChannelName() + " " + mode_str + " " + target_nick + "\r\n";
    channel.messageToMembers(client, "MODE", mode_msg);
}


void CommandHandler::handleChannelMode(Channel &channel, Command const &cmd, Client &client)
{
    std::vector<std::string> params = cmd.getParams();
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
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
                case 'o': // 채널 운영자 설정
                    handleChannelOperatorMode(channel, client, params, add);
                    break;
                // 다른 채널 모드 처리 (예: i, k, l 등)
                // case 'i': handleInviteOnlyMode(...);
                // case 'k': handleKeyMode(...);
                default:
                    _reply += ":localhost 472 " + client.getNickname() + " " + modes[i] + " :is unknown mode character \r\n";
                    break;
            }
        }
    }
}

void CommandHandler::handleUserMode(Client &client, Command const &cmd)
{
    std::vector<std::string> params = cmd.getParams();
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
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
                    _reply += ":localhost 501 " + client.getNickname() + " :Unknown MODE flag\r\n";
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
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    std::string target = params[0];
    if (target[0] == '#')
    {
        // 채널 모드 처리
        std::map<std::string, Channel*> &channels = server.getChannels();
        if (channels.find(target) != channels.end())
        {
            Channel &channel = *channels[target];
            handleChannelMode(channel, cmd, client);
        }
        else
        {
            // ERR_NOSUCHCHANNEL
            _reply += ":localhost 403 " + client.getNickname() + " " + target + " :No such channel\r\n";
        }
    }
    else
    {
        // 사용자 모드 처리
        handleUserMode(client, cmd);
    }
}
