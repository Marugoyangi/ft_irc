#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string myname) : _channel_name(myname), _topic(""), _topic_time(0), _mode(0), _key(""), _limit(-1){}

Channel::~Channel() {}

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

int	Channel::addClient(Client client)
{
	for (int i=0; i < (int)_fdlist.size() ; i++)
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

std::string	Channel::getChannelMembers(Server &server) const   //chan operator 앞에 @ 붙이기 처리해야됨
{

	if (server.getClients().size() < 1)
		return ("");
	if (server.getClients().find(_fdlist[0]) == server.getClients().end())
		return ("");
	std::string ret = server.getClients().at(_fdlist[0]).getNickname() + " ";
	for (int i = 1 ; i < (int)_fdlist.size(); i++)
	{
		if (server.getClients().find(_fdlist[i]) == server.getClients().end())
			return ("");
		ret += server.getClients().at(_fdlist[i]).getNickname() + " ";
	}
	return (ret);
}

void	Channel::messageToMembers(Client const &client, std::string cmd, std::string param)
{
	std::string msg;

	msg = client.getSource() + " " + cmd + " :" + param + "\r\n";
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			continue;
		std::cout << "\033[01m\033[33mmessage to client " << _fdlist[i] << ": "  << msg << "\033[0m" << std::endl; // debug
		if (isMember(_fdlist[i]))
			send(_fdlist[i], msg.c_str(), msg.length(), 0);
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
	return _operators.find(client.getNickname()) != _operators.end();
}

Client* Channel::getClient(const std::string &nickname)
{
	// need fix
	(void)nickname;
	return NULL;
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
