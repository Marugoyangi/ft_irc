#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string myname, Server &server) : _channel_name(myname), _topic(""), _topic_time(0), _mode(0b00000000), _key(""), _limit(-1), _server(&server)
{
	std::cout << "\033[32mChannel Created \033[0m" << std::endl;  //Debug
	_operators.insert("BOT");
}

Channel::~Channel() 
{
	std::cout << "\033[32mChannel Destroyed \033[0m" << std::endl;  //Debug
}

Channel::Channel(const Channel &other)
{
	*this = other;
}

Channel &Channel::operator=(const Channel &other)
{
	if (this == &other)
		return (*this);
	_channel_name = other._channel_name;
	for (int i = 0 ; i < (int)other._fdlist.size(); i++)
		_fdlist.push_back(other._fdlist[i]);
	for (std::set<std::string>::iterator it = other._operators.begin(); it != other._operators.end(); it++)
		_operators.insert(*it);
	for (std::set<std::string>::iterator it = other._invited_list.begin(); it != other._invited_list.end(); it++)
		_invited_list.insert(*it);
	_topic = other._topic;
	_topic_time = other._topic_time;
	_topic_setter = other._topic_setter;
	_topic_setter_source = other._topic_setter_source;
	_mode = other._mode;
	_key = other._key;
	_limit = other._limit;
	_server = other._server;
	return (*this);
}

////////////////////////////////////////////////////////////////
///OCCF //////////////////////////////////////

std::string	Channel::getChannelName() const
{
	return (_channel_name);
}

std::string	Channel::getChannelTopic() const
{
	return (_topic);
}

time_t	Channel::getTopicTime() const
{
	return (_topic_time);
}

void	Channel::setChannelTopic(std::string name)
{
	_topic = name;
	_topic_time = time(NULL);
}

bool	Channel::isMember(int fd) const
{
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		if (_fdlist[i] == fd)
			return (true);
	}
	return (false);
}

int	Channel::addClient(Client &client)
{
	for (int i = 0; i < (int)_fdlist.size() ; i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			return (-1);
	}
	_fdlist.push_back(client.getSocket_fd());
	return (0);
}

void	Channel::removeClient(int fd)
{
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		if (_fdlist[i] == fd)
		{
			_fdlist.erase(_fdlist.begin() + i);
			return;
		}
	}
}

void	Channel::removeOperator(std::string nickname)
{
	_operators.erase(nickname);
}

void	Channel::removeInvited(std::string nickname)
{
	_invited_list.erase(nickname);
}

std::string	Channel::getChannelMembers(Channel const &channel, Server &server) const
{
	std::string members;

	std::map<int, Client> &clients = server.getClients();
	members += "@BOT ";
	std::vector<int>::const_iterator it = channel._fdlist.begin();
	while (it != channel._fdlist.end())
	{
		if (clients.find(*it) != clients.end())
		{
			if (isOperator(clients.at(*it)))
				members += "@";
			members += clients.at(*it).getNickname();
			if (it + 1 != channel._fdlist.end())
				members += " ";
		}
		it++;
	}
	return members;
}

void	Channel::messageToMembers(Client const &client, std::string cmd, std::string param)
{
	std::string msg;

	msg = client.getSource() + " " + cmd + " :" + param + "\r\n";
	for (int i = 0; i < (int)_fdlist.size(); i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			continue;
		std::cout << "\033[01m\033[33mmessage to client " << _fdlist[i] << ": "  << msg << "\033[0m" << std::endl; // debug
		if (isMember(_fdlist[i]))
			_server->getClients().at(_fdlist[i]).getHandler().addReply(msg);
	}
}

void	Channel::messageToMembersIncludeSelf(Client const &client, std::string cmd, std::string param)
{
	std::string msg;

	msg = client.getSource() + " " + cmd + " :" + param + "\r\n";
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		std::cout << "\033[01m\033[33mmessage to client " << _fdlist[i] << ": "  << msg << "\033[0m" << std::endl; // debug
		if (isMember(_fdlist[i]))
			_server->getClients().at(_fdlist[i]).getHandler().addReply(msg);
	}
}

void	Channel::showChannelMembers(Server &server)  // for Debug
{
	std::cout << _channel_name << ": ";
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		if (server.getClients().find(_fdlist[0]) == server.getClients().end())
			return ;
		std::cout << server.getClients().at(_fdlist[i]).getNickname() << " ";
	}
	std::cout << std::endl;
}

void Channel::setOperator(Client &client, bool enable)
{
    if (enable)
        _operators.insert(client.getNickname());
    else
        _operators.erase(client.getNickname());
}

bool Channel::isOperator(const Client &client) const
{
	if (_operators.find(client.getNickname()) == _operators.end())
		std::cout << "not operator" << std::endl;
	else
		std::cout << "is operator" << std::endl;
	return _operators.find(client.getNickname()) != _operators.end();
}

void Channel::setMode(int mode)
{
	_mode |= mode;
}

void Channel::unsetMode(int mode)
{
	_mode &= ~mode;
}

bool Channel::isMode(int mode) const
{
	return _mode & mode;
}

void Channel::setKey(std::string key)
{
	_key = key;
}

std::string Channel::getKey() const
{
	return _key;
}

void Channel::setLimit(int limit)
{
	_limit = limit;
}

int Channel::getLimit() const
{
	return _limit;
}

Server &Channel::getServer() const
{
	return *_server;
}

void	Channel::addInvitedList(std::string client_name)
{
	_invited_list.insert(client_name);
}

bool Channel::checkInvitedList(Client &client)
{
    std::string nickname = client.getNickname();

    // _invited_list에 nickname이 존재하는지 확인
    if (_invited_list.find(nickname) != _invited_list.end()) {
        return true;
    }
    return false;
}

void Channel::setTopicTime(time_t time, std::string topic_setter, std::string source)
{
	_topic_time = time;
	_topic_setter = topic_setter;
	_topic_setter_source = source;
}

std::vector<int> &Channel::getFdList()
{
	return _fdlist;
}

void Channel::botmessageToMembers(std::string msg) const
{
	for (int i = 0; i < (int)_fdlist.size(); i++)
	{
		if (isMember(_fdlist[i]))
		{
			_server->getClients().at(_fdlist[i]).getHandler().addReply(msg);
			std::cout << "\033[01m\033[33mmessage to client " << _fdlist[i] << ": "  << msg << "\033[0m" << std::endl;
		}
	}
}

std::string Channel::getTopicSetter() const
{
	return _topic_setter;
}

std::string Channel::getTopicSetterSource() const
{
	return _topic_setter_source;
}

bool Channel::isInvited(std::string nickname)
{
	if (_invited_list.find(nickname) != _invited_list.end())
		return true;
	return false;
}