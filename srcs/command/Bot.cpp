#include "CommandHandler.hpp"

void CommandHandler::botHelp(Client const &client, Channel const &channel)
{
    std::string msg = ":BOT!~Bot@irc.local PRIVMSG " + channel.getChannelName() + \
    " :\033[32m@" + client.getNickname() + "\033[0m [Available commands]: ";
    msg += "JOIN, PART, MODE, TOPIC, NAMES, INVITE, KICK, QUIT, EXIT ";
    msg += "and [BOT commands]: !time, !uptime\r\n";
    channel.botmessageToMembers(msg);
}

void CommandHandler::botTime(Client const &client, Channel const &channel)
{
    (void)client;
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
    std::string msg = ":BOT!~Bot@irc.local PRIVMSG " + channel.getChannelName() + \
    " :\033[32m@" + client.getNickname() + "\033[0m Current time is ";
    msg += buffer;
    msg += "\r\n";
    channel.botmessageToMembers(msg);
}

void CommandHandler::botUptime(Client const &client, Channel const &channel)
{
    time_t rawtime;
    std::string msg;

    msg = ":BOT!~Bot@irc.local PRIVMSG " + channel.getChannelName() + \
    " :\033[32m@" + client.getNickname() + "\033[0m Uptime is ";
    rawtime = time(NULL);
    rawtime = rawtime - client.getEstablished_time();
    int days = rawtime / 86400;
    int hours = (rawtime % 86400) / 3600;
    int minutes = (rawtime % 3600) / 60;
    int seconds = rawtime % 60;
    std::stringstream ss;
    ss << days << " days, " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds";
    msg += ss.str();
    msg += "\r\n";
    channel.botmessageToMembers(msg);
}