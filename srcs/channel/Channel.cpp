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

std::string	Channel::getChannelName() const
{
	return (_channel_name);
}

bool	Channel::isMember(int fd)
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

std::string	Channel::getChannelMembers(Server &server) const
{
	if (server.getClients().size() < 1)
		return ("");
	std::string ret = server.getClients().at(_fdlist[0]).getNickname() + " ";
	for (int i = 1 ; i < (int)_fdlist.size(); i++)
	{
		ret += server.getClients().at(_fdlist[i]).getNickname() + " ";
	}
	return (ret);
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

void	Channel::messageToMembers(Client const &client, std::string cmd, std::string message)
{
	std::string tag = "";
	std::vector<std::string>	_params;
	Command	cmd_to_send;
	std::string source = client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();

	_params.push_back(message);
	cmd_to_send.setCommand("", source, cmd, _params);
	for (int i = 0 ; i < (int)_fdlist.size(); i++)
	{
		if (_fdlist[i] == client.getSocket_fd())
			continue;
		std::cout << "\033[01m\033[33mmessage to client: " << cmd_to_send.deparseCommand() << "\033[0m" << std::endl;
		send(_fdlist[i], cmd_to_send.deparseCommand().c_str(), cmd_to_send.deparseCommand().length(), 0);
	}
}


