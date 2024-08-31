#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Client *client) : _client(client)
{
}

CommandHandler::~CommandHandler() {}

CommandHandler::CommandHandler(const CommandHandler &other)
{
    *this = other;
}

CommandHandler &CommandHandler::operator=(const CommandHandler &other)
{
    if (this == &other)
        return (*this);
    return (*this);
}

void CommandHandler::execute(Command &cmd, Client &client, Server &server)
{
    std::string command = cmd.getCommand();
    _reply = "";
    
    if (client.getIs_registered() == false)
    {
        if (command == "PASS")
            pass(cmd, client);
        else if (command == "NICK")
            nick(cmd, client);
        else if (command == "USER")
            user(cmd, client);
        else
            reply(451, "", "you are not registered");
    }
    else ///////////////////// when client is registered
    {
        if (command == "PASS")
        {
            // PASS command
        }
        else if (command == "NICK")
        {
            // NICK command
        }
        else if (command == "USER")
        {
            // USER command
        }
        else if (command == "QUIT")
        {
            // QUIT command
        }
        else if (command == "JOIN")
        {
			join(cmd, client, server);
        }
        else if (command == "PART")
        {
            part(cmd, client, server.getChannels());
        }
        else if (command == "MODE")
        {
            // MODE command
        }
        else if (command == "TOPIC")
        {
            topic(cmd, client, server);
        }
        else if (command == "NAMES")
        {
            names(cmd, client, server);
        }
        else if (command == "LIST")
        {
            // LIST command
        }
        else if (command == "INVITE")
        {
            // INVITE command
        }
        else if (command == "KICK")
        {
            // KICK command
        }
        else if (command == "PRIVMSG")
        {
            // PRIVMSG command
        }
        else if (command == "NOTICE")
        {
            // NOTICE command
        }
        else if (command == "PING")
        {
            _reply = ":irc.local PONG irc.local :irc.local\r\n";
        }
        else if (command == "PONG")
        {
            // PONG command
        }
        else if (command == "WHO")
        {
            // WHO command
        }
        else if (command == "WHOIS")
        {
            // WHOIS command
        }
        else if (command == "WHOWAS")
        {
            // WHOWAS command
        }
        else if (command == "ISON")
        {
            // ISON command
        }
        else
        {
            // ERR_UNKNOWNCOMMAND
        }
    }
    // client.showClient();
	std::cout << "\033[01m\033[33mmessage to client " << client.getSocket_fd() << ": "  << _reply << "\033[0m" << std::endl;
    send(client.getSocket_fd(), _reply.c_str(), _reply.length(), 0);
}

void CommandHandler::reply(int numeric, std::string param, std::string message)
{
    std::string reply;
    std::string head;
    std::string tail = ":" + message + "\r\n";
    std::stringstream ss;
    ss << numeric;
    if (numeric == 0)
        head = "";
    else
        head = ss.str();
    reply = head + " " + param + " " + tail;
    _reply += reply;
}

void CommandHandler::reply(std::string const &command, std::string const &message)
{
    _reply += ":irc.local " + command + " :" + message + "\r\n";
}

void CommandHandler::reply(std::string const &command, std::vector<std::string> const &message)
{
    _reply += ":irc.local " + command;
    for (size_t i = 0; i < message.size(); i++)
    {
        _reply += " ";
        if (i == message.size() - 1)
            _reply += ":";
        _reply += message[i];
    }
    _reply += "\r\n";
}


// all those commands

void CommandHandler::pass(Command &cmd, Client &client)
{
    if (client.getIs_passed() == true)
    {
        reply(462, "", "You may not reregister");
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
            reply(464, "","Password incorrect");
            return;
        }
    }
    if (cmd.getParams().size() == 0)
    {
        reply(431, "", "No nickname given");
        return;
    }
    if (cmd.getParams()[0].size() > 9 || cmd.getParams()[0].size() < 1 || 
    cmd.getParams()[0].find_first_not_of(\
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-") != std::string::npos)
    {
        reply(432, "", "Erroneous nickname");
        return;
    }
    std::string nickname = cmd.getParams()[0];
    std::set<std::string> nicknames = client.getServer()->getNicknames();
    if (nicknames.find(nickname) != nicknames.end())
    {
        reply(433, nickname, "Nickname is already in use");
        return;
    }
    std::string str = cmd.getParams()[0];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setNickname(str);
}

void CommandHandler::user(Command &cmd, Client &client)
{
    // if (client.getIs_passed() == false)
    // {
    //     if (client.getTry_password() == client.getPassword())
    //         client.setIs_passed(true);
    //     else
    //     {
    //         reply(464, "","Password incorrect");
    //         return;
    //     }
    // }
    client.setIs_passed(true);

    if (cmd.getParams().size() < 4)
    {
        reply(461, "USER", "Not enough parameters");
        return;
    }
    if (client.getNickname() == "") // Nickname not set
    {
        reply(462, "", "You may not reregister");
        return;
    }
    // // 암호와 인자 유효한 경우엔 Ident 프로토콜 실행
    // std::string identified_user = "";
    // std::string ident_server = "";

    // ident_server = client.getServer()->getPort() + ",113\r\n";
    // ssize_t sent = send(client.getSocket_fd(), ident_server.c_str(), ident_server.length(), 0);
    // if (sent == -1)
    // {
    //     die("send");
    //     return;
    // }
    // time_t start = time(NULL);
    // while(1)
    // {
    //     if (time(NULL) - start > 10)
    //         break ;
    //     char buffer[BUFFER_SIZE];
    //     ssize_t received = recv(client.getSocket_fd(), buffer, BUFFER_SIZE, 0);
    //     if (received == -1)
    //     {
    //         die("recv");
    //         return;
    //     }
    //     if (received == 0)
    //         break;
    //     std::string message(buffer, received);
    //     std::string::size_type pos = message.find("USERID");
    //     if (pos != std::string::npos)
    //     {
    //         identified_user = message.substr(pos + 7, message.find(" ", pos + 7) - pos - 7);
    //         break;
    //     }
    // }
    // if (identified_user == "") // debug
    //     printf("Debug: Ident_serv: No USERID received\n");
    if (cmd.getParams()[0].size() > 9 || cmd.getParams()[0].size() < 1 ||
    cmd.getParams()[3].size() > 50)
    {
        reply(461, "USER", "Not enough parameters");
        return;
    }
    if (cmd.getParams()[2] != "*")
        client.setHostname(cmd.getParams()[2]); // debug purpose
    std::string str = "~" + cmd.getParams()[0]; // tilde means custom ident
    // if (identified_user != "")
    //     str = identified_user;
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setUsername(str);
    str = cmd.getParams()[3];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setRealname(str);
    // ping-pong should be turned off until the user is registered
    welcome(client);
    client.setIs_registered(true);
}

void CommandHandler::welcome(Client &client)
{
    std::string server_name = client.getServer()->getServerName();
    com001(client, server_name);
    // reply(001, "", "Welcome to the " + server_name + " Network, " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname());
    reply(002, "", "Your host is " + server_name + ", running version " + "ircserv 1.0");
    reply(003, "", "This server was created sometime"); // need fix
    reply(004, server_name + " ircserv 1.0 abhi bhi", "ao");
    std::string modes =
        "CASEMAPPING=rfc1459 CHARSET=ascii NICKLEN=9 CHANNELLEN=50 TOPICLEN=390 "
        "CHANTYPES=#& PREFIX=(ov)@+ MAXLIST=bqeI:100 MODES=4 NETWORK=" + server_name;
    reply(005, "", modes);
    reply(251, "", "There are 1 users and 0 services on 1 servers");
    reply(252, "", "0 :operator(s) online");
    reply(253, "", "0 :unknown connection(s)");
    reply(254, "", "0 :channels formed");
    reply(255, "", "I have 1 clients and 0 servers");
    reply(265, "", "0 1 :Current local users 0, max 1");
    reply(266, "", "1 1 :Current global users 1, max 1");
    reply(375, "", ":- " + server_name + " Message of the Day - ");
    reply(372, "", ":- Welcome to the " + server_name + " IRC Network");
    reply(376, "", "End of /MOTD command");
}

