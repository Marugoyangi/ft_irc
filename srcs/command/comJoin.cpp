#include "CommandHandler.hpp"

// void	comJoin(Client const &client, std::vector<std::string> const &param)
void	CommandHandler::join(Command &cmd, Client &client, std::map<std::string, Channel> &channels)
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
			channel_name = cmd.getParams()[0].substr(0, comma_pos);
			cmd.getParams()[0].erase(0, comma_pos + 1);
		}
		if (cmd.getParams().size() > 1)
		{
			mode_pos = cmd.getParams()[2].find(',');
			if (mode_pos > 0)
			{
				mode = cmd.getParams()[1].substr(0, mode_pos);
				cmd.getParams()[1].erase(0, mode_pos + 1);
			}
		}
		std::map<std::string, Channel>::iterator it = channels.find(channel_name);
		if (channels.find(channel_name) == channels.end())
			channels[channel_name] = *(new Channel);
		if (channels[channel_name].addClientToChannel(client) == -1)
			continue;
		// 1. ban 리스트에 nickname, username, hostname 있는지 비교
		// 2. 비밀번호 확인?
		
		// _tem.push_back(param[0]);
		// _cmd.setCommand("", client.get_hostname(), "JOIN", _tem);
		// _message = _cmd.deparseCommand();
		// _message += com332(client, _cmd);
		// _message += com333(client, _cmd);
		// _message += com353(client, _cmd);
		// _message += com366(client, _cmd);
	}
}

std::string	com461(Client const &client, Command const &cmd)
{
	Command _cmd;
	std::vector<std::string> _tem;

	_tem.push_back(client.get_username());
	_tem.push_back(cmd.getCommand());
	_tem.push_back("Not enough parameters");
	_cmd.setCommand("", client.get_username(), "461", _tem);
	return (_cmd.deparseCommand());
}

std::string	com332(Client const &client, Command const &cmd)
{
}

std::string	com333(Client const &client, Command const &cmd)
{
}

std::string	com353(Client const &client, Command const &cmd)
{
}

std::string	com366(Client const &client, Command const &cmd)
{
}