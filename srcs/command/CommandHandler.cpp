#include "CommandHandler.hpp"

CommandHandler::CommandHandler()
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

void CommandHandler::execute(Command &cmd, Client &client)
{
    std::string command = cmd.getCommand();
    std::string _message;
    std::vector<std::string> _tem;

    if (client.get_is_registered() == false)
    {
        if (command == "PASS")
            client.set_try_password(cmd.getMessage());
        else if (command == "NICK")
        {
            if (client.get_try_password() == client.get_password())
            {
                if (client.get_username() == "")
                    client.set_username(cmd.getMessage());
                else
                    reply(client.get_socket_fd(), 433, "ERR_NICKNAMEINUSE");
            }
            else
                reply(client.get_socket_fd(), 464, "ERR_PASSWDMISMATCH");
        }
        else if (command == "USER")
        {
            if (client.get_try_password() == client.get_password())
            {
                if (client.get_username() == "")
                    reply(client.get_socket_fd(), 451, "ERR_NOTREGISTERED");
                else
                {
                    client.set_realname(cmd.getMessage());
                    client.set_is_registered(true);
                }
            }
            else
                reply(client.get_socket_fd(), 464, "ERR_PASSWDMISMATCH");
        }
        else
        {
            reply(client.get_socket_fd(), 451, "ERR_NOTREGISTERED");
        }
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
            if (cmd.getParameter().size() < 1)
            {
                _tem.push_back("JOIN");
                _message += com461(client, _cmd);
                return;
            }
            _tem.push_back(param[0]);
            _cmd.setCommand("", client.get_hostname(), "JOIN", _tem);
            _message = _cmd.deparseCommand();
            _message += com332(client, _cmd);
            _message += com333(client, _cmd);
            _message += com353(client, _cmd);
            _message += com366(client, _cmd);
        }
        else if (command == "PART")
        {
            // PART command
        }
        else if (command == "MODE")
        {
            // MODE command
        }
        else if (command == "TOPIC")
        {
            // TOPIC command
        }
        else if (command == "NAMES")
        {
            // NAMES command
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
            // PING command
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
    client.showClient();
}

void CommandHandler::reply(int fd, int numeric, std::string message)
{
    std::string reply;
    std::string tail = ":" + message + "\r\n";
    std::stringstream ss;
    ss << numeric;
    if (numeric == 0)
    {
        reply = ":irc.local " + tail;
    }
    else
    {
        reply = ":irc.local " + ss.str() + " " + tail;
    }
    send(fd, reply.c_str(), reply.length(), 0);
}
