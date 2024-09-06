#include "CommandHandler.hpp"

void CommandHandler::handleChannelInviteMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }
    if (add)
        channel.setMode(MODE_I);
    else
        channel.unsetMode(MODE_I);
    // 모드 변경 메시지
	std::string mode_str = add ? "+i" : "-i";
	std::string mode_msg = ":irc.local MODE " + channel.getChannelName() + " " + mode_str + "\r\n";
    send(client.getSocket_fd(), mode_msg.c_str(), mode_msg.length(), 0);
	channel.messageToMembers(client, "MODE " + channel.getChannelName() + " " + mode_str, "");
}

void CommandHandler::handleChannelKeyMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if ((params.size() < 3 && add) || (params.size() < 2 && !add))
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }
    std::string key = params[2];
    if (add)
    {
        channel.setKey(key);
        channel.setMode(MODE_K);
    }
    else
    {
        channel.setKey("");
        channel.unsetMode(MODE_K);
    }
    // 모드 변경 메시지
    std::string mode_str = add ? "+k" : "-k";
    std::string mode_msg = ":irc.local MODE " + channel.getChannelName() + " " + mode_str + "\r\n";
    send(client.getSocket_fd(), mode_msg.c_str(), mode_msg.length(), 0);
	channel.messageToMembers(client, "MODE " + channel.getChannelName() + " " + mode_str, "");
}

void  CommandHandler::handleChannelLimitMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if ((params.size() < 3 && add) || (params.size() < 2 && !add))
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }
    if (add)
    {
        std::stringstream ss(params[2]);
        int limit;
        ss >> limit;
        channel.setLimit(limit);
        channel.setMode(MODE_L);
    }
    else
    {
        channel.setLimit(-1);
        channel.unsetMode(MODE_L);
    }
    // 모드 변경 메시지
    std::string mode_str = add ? "+l" : "-l";
    if (add)
        mode_str += " " + params[2];
    std::string mode_msg = ":irc.local MODE " + channel.getChannelName() + " " + mode_str + "\r\n";
    send(client.getSocket_fd(), mode_msg.c_str(), mode_msg.length(), 0);
	channel.messageToMembers(client, "MODE " + channel.getChannelName() + " " + mode_str, "");
}

void CommandHandler::handleChannelTopicMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add)
{
    if (params.size() < 2)
    {
        // ERR_NEEDMOREPARAMS
        _reply += ":localhost 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        return;
    }
    if (add)
        channel.setMode(MODE_T);
    else
        channel.unsetMode(MODE_T);
    // 모드 변경 메시지
    std::string mode_str = add ? "+t" : "-t";
    std::string mode_msg = ":irc.local MODE " + channel.getChannelName() + " " + mode_str + "\r\n";
    send(client.getSocket_fd(), mode_msg.c_str(), mode_msg.length(), 0);
	channel.messageToMembers(client, "MODE " + channel.getChannelName() + " " + mode_str, "");
}