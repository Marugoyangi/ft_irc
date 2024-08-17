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
    (void)client;
    std::string command = cmd.getCommand();
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
                    ; // ERR_ALREADYREGISTERED
            }
            else
                std::cout << "ERR_PASSWDMISMATCH" << std::endl;
        }
        else if (command == "USER")
        {
            if (client.get_try_password() == client.get_password())
            {
                if (client.get_username() == "")
                    ;// ERR_NEEDMOREPARAMS
                else
                {
                    client.set_realname(cmd.getMessage());
                    client.set_is_registered(true);
                }
            }
            else
                std::cout << "ERR_PASSWDMISMATCH" << std::endl;
        }
        else
        {
            ;// ERR_NOTREGISTERED
        }
    }
    else
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
            // JOIN command
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
