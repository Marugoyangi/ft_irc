#include "CommandHandler.hpp"

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