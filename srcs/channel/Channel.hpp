#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <iostream>
# include <set>

class Server;
class Client;

class Channel
{
	private :
		std::string			_channel_name;
		std::string			_topic;
		time_t				_topic_time;
		std::set<std::string> _operators;
		std::vector<int>	_fdlist;

	public :
		Channel();
		~Channel();
		Channel &operator=(const Channel &other);
		Channel(const Channel &other);

		Channel(std::string myname);
		
		std::string	getChannelName() const;
		std::string	getChannelMembers(Server &server) const;
		std::string	getChannelTopic() const;
		time_t getTopicTime() const;

		void	setChannelTopic(std::string name);
		bool	isMember(int fd) const;
		int		addClient(Client client);
		void	removeClient(int fd);

		void 	setOperator(Client &client, bool enable);
    	bool 	isOperator(const Client &client) const;
    	void 	messageToMembers(Client const &client, std::string cmd, std::string param);
		Client* getClient(const std::string &nickname);

		void	showChannelMembers(Server &server);  // for Debug
};

# include "../server/Server.hpp"
# include "../client/Client.hpp"

#endif
