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

	channel.setMode(MODE_O);

    // 모드 변경 메시지
    std::string mode_str = add ? "+o" : "-o";
    std::string mode_msg = ":" + client.getNickname() + " MODE " + channel.getChannelName() + " " + mode_str + " " + target_nick + "\r\n";
    channel.messageToMembers(client, "MODE", mode_msg);
}

void CommandHandler::handleChannelInviteMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }

    channel.setMode(MODE_I);

    // 모드 변경 메시지
	std::string mode_str = add ? "+i" : "-i";
	std::string mode_msg = ":" + client.getNickname() + " MODE " + channel.getChannelName() + " " + mode_str + "\r\n";

	channel.messageToMembers(client, "MODE", mode_msg);
}