#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <iostream>

class Server;
class Client;

class Channel
{
	private :
		std::string _channel_name;
		std::vector<int> _fdlist;

	public :
		Channel();
		~Channel();
		Channel &operator=(const Channel &other);
		Channel(const Channel &other);

		Channel(std::string myname);
		
		std::string	getChannelName() const;
		std::string	getChannelMembers(Server &server) const;
		bool		isMember(int fd);
		int			addClient(Client client);
		void		removeClient(int fd);
		void		messageToMembers(Client const &client, std::string cmd, std::string message);

		void		showChannelMembers(Server &server);
};

# include "../server/Server.hpp"
# include "../client/Client.hpp"

#endif
