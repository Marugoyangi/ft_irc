#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string myname) : _channel_name(myname) {}

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

int	Channel::addClientToChannel(Client client)
{
	for (int i=0; i < (int)_fdlist.size() ; i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			return (-1);
	}
	_fdlist.push_back(client.getSocket_fd());
	return (0);
}

void	Channel::showChannelMembers(Server &server)
{
	std::cout << _channel_name << ": ";
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		std::cout << server.getClients().at(_fdlist[i]).getNickname() << " ";
	}
	std::cout << std::endl;
}

