#include "Command.hpp"

// OCCF START
Command::Command() 
{
	_fd = -1;
	_command = "";
	_tag = "";
	_source = "";
}

Command::~Command() {}

Command::Command(const Command &other)
{
	*this = other;
}

Command &Command::operator=(const Command &other)
{
	if (this == &other)
		return (*this);
	_fd = other._fd;
	_command = other._command;
	_tag = other._tag;
	_source = other._source;
	_parameter = other._parameter;
	return (*this);
}
//OCCF END

void	Command::clearCommand()
{
	_fd = -1;
	_command = "";
	_tag = "";
	_source = "";
	_parameter.clear();
}

void	Command::parseCommand(std::string command, int fd)
{
	_fd = fd;
	int	pos;
	std::vector<std::string> spl;
	
	pos = 0;
	while ((pos=command.find(" ", pos)) != (int)std::string::npos)
	{
		spl.push_back(command.substr(0, pos));
		command = command.substr(pos + 1);
	}
	spl.push_back(command);
	for (int idx = 0; idx < (int)spl.size(); idx++)
	{
		if (idx == 0 && spl[idx][0] == '@')
		{
			_tag = spl[idx].substr(1);
			if (spl.size() > 1 && spl[idx + 1][0] == ':')
			{
				_source = spl[idx + 1].substr(1);
				idx++;
			}
		}
		else if (idx == 0 && spl[idx][0] == ':')
			_source = spl[idx].substr(1);
		else if (spl[idx][0] == ':' && _command != "")
		{
			std::string tem = "";

			tem = spl[idx].substr(1);
			idx++;
			while (idx < (int)spl.size())
			{
				tem += " " + spl[idx];
				idx++;
			}
			_parameter.push_back(tem);
			break;
		}
		else if (_command == "")
			_command = spl[idx];
		else
			_parameter.push_back(spl[idx]);
	}	
}

void	Command::showCommand()
{
	std::cout << "\t\tTag : " << _tag << std::endl;
	std::cout << "\t\tSource : " << _source << std::endl;
	std::cout << "\t\tCommand : " << _command << std::endl;
	std::cout << "\t\tParameter : ";
	for (int idx = 0; idx < (int)_parameter.size(); idx++)
		std::cout << _parameter[idx] << ", ";
	std::cout << std::endl;
}

void	Command::execCommand(Client const *clientlist)
{
	if (_command == "NICK")
	{
		// NICK
	}
	else if (_command == "USER")
	{
		// USER
	}
	else if (_command == "QUIT")
	{
		// QUIT
	}
	else if (_command == "JOIN")
	{
		// JOIN
	}
}