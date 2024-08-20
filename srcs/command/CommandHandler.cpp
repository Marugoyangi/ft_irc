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

void CommandHandler::execute(Command &cmd, Client &client)
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
			std::string	channel_name;
			std::string	mode;
			size_t		comma_pos;
			size_t		mode_pos;

            if (cmd.getParams().size() < 1)
            {
				reply(461, "JOIN", "Not enough parameters");
                return;
            }
			while ((comma_pos = cmd.getParams()[1].find(',')) != std::string::npos)
			{
				if (comma_pos > 0)
				{
					channel_name = command.substr(0, comma_pos);
					command.erase(0, comma_pos + 1);
				}
				if (cmd.getParams().size() > 1)
				{
					mode_pos = cmd.getParams()[2].find(',');
					if (mode_pos > 0)
					{
						mode = command.substr(0, mode_pos);
						command.erase(0, mode_pos + 1);
					}
				}
				// 1. 채널 없으면 만들고 있으면 invite
				// 2. ban 리스트에 nickname, username, hostname 있는지 비교
				// 3. 비밀번호 확인?
				 
				_tem.push_back(param[0]);
				_cmd.setCommand("", client.get_hostname(), "JOIN", _tem);
				_message = _cmd.deparseCommand();
				_message += com332(client, _cmd);
				_message += com333(client, _cmd);
				_message += com353(client, _cmd);
				_message += com366(client, _cmd);
			}
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
    if (_client->getServer()->getNicknames().find(cmd.getParams()[0]) != \
    _client->getServer()->getNicknames().end())
    {
        reply(433, cmd.getParams()[0], "Nickname is already in use"); // 이거 왜 지맘대로 있다고뜨나
        return;
    }
    std::string str = cmd.getParams()[0];
    std::transform(str.begin(), str.end(), str.begin(),
                   static_cast<int(*)(int)>(std::tolower));
    _client->setUsername(str);
}

void CommandHandler::user(Command &cmd, Client &client)
{
    (void) cmd;
    (void) client;
    // 어우 머리아퍼
}

