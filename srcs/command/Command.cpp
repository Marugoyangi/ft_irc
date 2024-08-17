#include "Command.hpp"

// OCCF START
Command::Command() 
{
	_numeric = 0;
	_command = "";
	_tag = "";
	_source = "";
	_parameter.clear();
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
	_numeric = other._numeric;
	_command = other._command;
	_tag = other._tag;
	_source = other._source;
	_parameter = other._parameter;
	return (*this);
}
//OCCF END

void	Command::clearCommand()
{
	_numeric = 0;
	_command = "";
	_tag = "";
	_source = "";
	_parameter.clear();
}

void Command::parseCommand(std::string command){
	std::vector<std::string> spl;
	size_t pos = 0;

	while ((pos = command.find(' ')) != std::string::npos) { // 공백을 기준으로 문자열을 나눔
		if (pos > 0) {
			// 공백이 아닌 부분을 벡터에 추가
			spl.push_back(command.substr(0, pos));
		}
		// 다음 부분으로 이동
		command.erase(0, pos + 1);
	}

	if (!command.empty()) // 마지막 부분 추가
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
		else if (spl[idx][0] == ':' && !_command.empty())
		{
			std::string tem = spl[idx].substr(1);
			idx++;
			while (idx < (int)spl.size())
			{
				tem += " " + spl[idx];
				idx++;
			}
			_parameter.push_back(tem);
			break;
		}
		else if (_command.empty())
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
	{
		if (idx == (int)_parameter.size() - 1)
			std::cout << _parameter[idx];
		else
			std::cout << _parameter[idx] << ", ";
	}
	std::cout << std::endl;
}

std::string	Command::getCommand()
{
	return (_command);
}

std::string	Command::getTag()
{
	return (_tag);
}

std::string Command::getMessage() {
    // 벡터가 비어있지 않다면 마지막 파라미터를 반환
    if (!_parameter.empty()) {
        return _parameter.back();
    } else {
        return ""; // 벡터가 비어있으면 빈 문자열 반환
    }
}
