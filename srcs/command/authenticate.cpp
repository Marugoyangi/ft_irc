#include "CommandHandler.hpp"

void CommandHandler::pass(Command &cmd, Client &client)
{
    if (client.getIs_passed() == true)
    {
        reply(462, "PASS", "You may not reregister");
        return;
    }
    if (cmd.getParams().size() == 0)
    {
        reply(461, "PASS", "Not enough parameters");
        return;
    }
    client.setTry_password(cmd.getParams()[0]);
}

void CommandHandler::nick(Command &cmd, Client &client)
{
    if (client.getIs_passed() == false)
    {
        if (client.getTry_password() == client.getPassword())
            client.setIs_passed(true);
        else
        {
            reply(464, "NICK","Password incorrect");
            return;
        }
    }
    if (cmd.getParams().size() == 0)
    {
        reply(431, "NICK", "No nickname given");
        return;
    }
    if (cmd.getParams()[0].size() > 9 || cmd.getParams()[0].size() < 1 || 
        cmd.getParams()[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_") != std::string::npos)
    {
        reply(432, "NICK", "Erroneous nickname");
        return;
    }
    std::string nickname = cmd.getParams()[0];
    std::set<std::string> nicknames = client.getServer()->getNicknames();
    if ((nicknames.find(nickname) != nicknames.end()) || nickname == "bot" || nickname == "BOT")
    {
        std::string tmp = "NICK " + nickname;
        reply(433, tmp, "Nickname is already in use");
        return;
    }
    std::string str = cmd.getParams()[0];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
	// 등록된 상태에서 nick 변경
	if (client.getIs_registered() == true)
	{
		if (client.getNickname() == nickname)
			return;
        std::transform(nickname.begin(), nickname.end(), nickname.begin(),
                   static_cast<int(*)(int)>(std::tolower));
		std::string msg = client.getSource() + " NICK :" + nickname + "\r\n";
		_reply += msg;
        std::cout << "\033[01m\033[33mmessage to client " << client.getSocket_fd() << ": "  << msg << "\033[0m" << std::endl;
        std::string old_nickname = client.getNickname();
		for (std::map<std::string, Channel>::iterator it=client.getServer()->getChannels().begin(); it != client.getServer()->getChannels().end(); it++)
		{
			it->second.messageToMembers(client, "NICK", nickname);
            _client->setNickname(nickname);
            if (it->second.isOperator(client))
            {
                it->second.removeOperator(old_nickname);
                it->second.setOperator(client, true);
                it->second.messageToMembers(client, "MODE", "+o " + nickname);
            }
            if (it->second.isInvited(old_nickname))
            {
                it->second.removeInvited(old_nickname);
                it->second.addInvitedList(nickname);
                it->second.messageToMembers(client, "MODE", "+i " + nickname);
            }
		}
        return ;
    }
    _client->setNickname(str);
}

void CommandHandler::user(Command &cmd, Client &client)
{
    if (client.getIs_passed() == false)
    {
        if (client.getTry_password() == client.getPassword())
            client.setIs_passed(true);
        else
        {
            reply(464, "USER","Password incorrect");
            return;
        }
    }
    client.setIs_passed(true);
    if (cmd.getParams().size() < 4)
    {
        reply(461, "USER", "Not enough parameters");
        return;
    }
	if (client.getNickname() == "") // Nickname not set
    {
        reply(462, "USER", "You may not reregister");
        return;
    }
    if (cmd.getParams()[0].size() < 1 || cmd.getParams()[3].size() > 50)
    {
        reply(461, "USER", "Not enough parameters");
        return;
    }
    client.setHostname(cmd.getParams()[2]);
    std::string str = "~" + cmd.getParams()[0]; // tilde means custom ident
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setUsername(str);
    str = cmd.getParams()[3];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setRealname(str);
	client.setIs_registered(true);
    welcome(client);
}

void CommandHandler::welcome(Client &client)
{
    std::string server_name = client.getServer()->getServerName();
    std::string client_name = client.getNickname();
    reply(001, client_name, "Welcome to the " + server_name + " IRC Network, " + client_name + "!" + \
    client.getUsername() + "@" + client.getHostname());
    reply(002, client_name, "Your host is " + server_name + ", running version 1.0");
    time_t time = client.getServer()->getLocalTime();
    char buffer[80];
    strftime(buffer, 80, "%a %b %d %H:%M:%S %Y", localtime(&time));
    reply(003, client_name, "This server was created " + std::string(buffer));
    reply(004, client_name, server_name + " 1.0 " + "o " + "itkol");
    std::stringstream modes;
    modes << "CASEMAPPING=rfc1459 CHARSET=ascii NICKLEN=9 TOPICLEN=390 " << "CHANTYPES=# PREFIX=(ov)@+ MODES=4 NETWORK=" << server_name << " MAXTARGETS=" << MAX_TARGETS << " :are supported by this server";
    reply(005, client_name, modes.str());
    std::stringstream ss;
    ss << "There are " << client.getServer()->getClients().size() << " clients, 1 services and 1 servers"; // bot service
    reply(251, client_name, ss.str());
    reply(252, client_name, "0 :operator(s) online");
    int unknown = 0;
    for (std::map<int, Client>::iterator it = client.getServer()->getClients().begin(); it != client.getServer()->getClients().end(); it++)
    {
        if (it->second.getIs_registered() == false)
            unknown++;
    }
    std::stringstream ss2;
    if (unknown == 0)
        ss2 << "0 :unknown connection(s)";
    else
        ss2 << unknown << " :unknown connection(s)";
    reply(253, client_name, ss2.str());
    ////////////////////////////////////////
    ss.str("");
    ss << client.getServer()->getChannels().size() << " :channels formed";
    reply(254, client_name, ss.str());
    ss.str("");
    ss << client.getServer()->getClients().size() << " :clients and 1 server";
    reply(255, client_name, ss.str());
    ss.str("");
    ss << client.getServer()->getClients().size() << " :Current local users " \
    << client.getServer()->getClients().size() << ", max " << MAX_CLIENTS;
    ss.str("");
    reply(265, client_name, ss.str());
    ss << client.getServer()->getClients().size() << " :Current global users " \
    << client.getServer()->getClients().size() << ", max " << MAX_CLIENTS;
    ss.str("");
    reply(266, client_name, ss.str());
    //need fix///////////////////////////////
    reply(375, client_name, ":- " + server_name + " Message of the Day - ");
    reply(372, client_name, ":- Welcome to the " + server_name + " IRC Network -:");
    reply(376, client_name, "End of message of the day.");
}