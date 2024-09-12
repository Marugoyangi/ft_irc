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
            quit(cmd, client, server.getChannels());
        }
        else if (command == "JOIN")
        {
			join(cmd, client, server);
        }
        else if (command == "PART")
        {
            part(cmd, client, server.getChannels());
            server.cleanChans();
        }
        else if (command == "MODE")
        {
            mode(cmd, client, server);
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
            invite(cmd, client, server);
        }
        else if (command == "KICK")
        {
            kick(cmd, client, server);
            server.cleanChans();
        }
        else if (command == "PRIVMSG")
        {
            privmsg(cmd, client, server);
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
    client.setLast_active_time(time(NULL));
    if (_reply != "")
    {
        send(client.getSocket_fd(), _reply.c_str(), _reply.length(), 0);
        std::cout << "\033[01m\033[33mmessage to client " << client.getSocket_fd() << ": "  << _reply << "\033[0m" << std::endl;
    }
}

void CommandHandler::reply(int numeric, std::string param, std::string message)
{
    std::string reply;
    std::string head;
    std::string tail = ":" + message + "\r\n";
    std::stringstream ss;
    ss << numeric;
    if (ss.str().length() == 1)
        head = "00" + ss.str();
    else if (ss.str().length() == 2)
        head = "0" + ss.str();
    else
        head = ss.str();
    reply = ":irc.local " + head + " " + param + " " + tail;
    std::cout << "reply: " << reply << std::endl;
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

void CommandHandler::reply(Client &client, const char* numeric, const std::string &channel_name, const std::string &message)
{
    std::string reply;
    std::string tail = ":" + message + "\r\n";

    // 클라이언트 닉네임과 함께 답변 생성
    reply = ":irc.local " + std::string(numeric) + " " + client.getNickname() + " " + channel_name + " " + tail;

    std::cout << "reply: " << reply << std::endl;

    // 실제로 전송할 _reply 변수에 결과 저장
    _reply += reply;
}

// all those commands

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
    if (nicknames.find(nickname) != nicknames.end())
    {
        std::string tmp = "NICK " + nickname;
        reply(433, tmp, "Nickname is already in use");
        return;
    }
    std::string str = cmd.getParams()[0];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
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
    // // 암호와 인자 유효한 경우엔 Ident 프로토콜 실행
    std::string identified_user = "";
    std::string ident_server = "";
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    if (getpeername(client.getSocket_fd(), (struct sockaddr *)&peer_addr, &peer_addr_len) == -1)
        return;
    char client_port[6];
    snprintf(client_port, 6, "%d", ntohs(peer_addr.sin_port));
    ident_server = client.getServer()->getPort() + "," + client_port;
    ssize_t sent = send(client.getSocket_fd(), ident_server.c_str(), ident_server.length(), 0);
    if (sent == -1)
        return;
    time_t start = time(NULL);
    while(1)
    {
        if (time(NULL) - start > 10)
            break ;
        char buffer[BUFFER_SIZE];
        ssize_t received = recv(client.getSocket_fd(), buffer, BUFFER_SIZE, 0);
        if (received == 0)
            break;
        if (received == -1)
            break;
        std::string message(buffer, received);
        std::string::size_type pos = message.find("USERID");
        if (pos != std::string::npos)
        {
            identified_user = message.substr(pos + 7, message.find(" ", pos + 7) - pos - 7);
            break;
        }
    }
    if (identified_user == "") // debug
        printf("Debug: Ident_serv: No USERID received\n");
    // if (cmd.getParams()[0].size() > 9 || cmd.getParams()[0].size() < 1 ||
    // cmd.getParams()[3].size() > 50)
    // {
    //     reply(461, "USER", "Not enough parameters");
    //     return;
    // }
    if (cmd.getParams()[2] != "*")
        client.setHostname(cmd.getParams()[2]); // debug purpose
    std::string str = "~" + cmd.getParams()[0]; // tilde means custom ident
    if (identified_user != "")
        str = identified_user;
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setUsername(str);
    str = cmd.getParams()[3];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setRealname(str);
    // ping-pong should be turned off until the user is registered
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
    modes << "CASEMAPPING=rfc1459 CHARSET=ascii NICKLEN=9 TOPICLEN=390 " << "CHANTYPES=# PREFIX=(o)@ MODES=4 NETWORK=" << server_name << " MAXTARGETS=" << MAX_TARGETS << " :are supported by this server";
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
