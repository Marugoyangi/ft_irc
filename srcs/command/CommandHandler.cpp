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
            // KICK command
        }
        else if (command == "PRIVMSG")
        {
            privmsg(cmd, client, server);
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
