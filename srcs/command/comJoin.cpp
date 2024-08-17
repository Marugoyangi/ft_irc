#include "Command.hpp"

void	comJoin(Client const &client, std::vector<std::string> const &param)
{
	Command						_cmd;
	std::string					_message;
	std::vector<std::string>	_tem;
	
	if (param.size() < 1)
	{
		_tem.push_back("JOIN");
		_message += com461(client, _cmd);
		return ;
	}
	_tem.push_back(param[0]);
	_cmd.setCommand("", client.get_hostname(), "JOIN", _tem);
	_message = _cmd.deparseCommand();
	_message += com332(client, _cmd);
	_message += com333(client, _cmd);
	_message += com353(client, _cmd);
	_message += com366(client, _cmd);
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