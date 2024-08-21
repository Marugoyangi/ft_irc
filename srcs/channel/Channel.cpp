#include "Channel.hpp"

Channel::Channel() {}

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
	for (int i = 0 ; i < other._fdlist.size(); i++)
		_fdlist.push_back(other._fdlist[i]);
}

////////////////////////////////////////////////////////////////
///OCCF //////////////////////////////////////

int	Channel::addClientToChannel(Client client)
{
	for (int i=0; i < _fdlist.size() ; i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			return (-1);
	}
	_fdlist.push_back(client.getSocket_fd());
	return (0);
}

