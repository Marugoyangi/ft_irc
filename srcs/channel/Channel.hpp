#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <iostream>
# include <set>

#define MODE_I 0x1 // invite only 0b00000001
#define MODE_T 0x2 // topic settable by channel operator only 0b00000010
#define MODE_K 0x4 // channel key required for entry 0b00000100
#define MODE_L 0x8 // limit the number of clients 0b00001000
// #define MODE_O 0x8 // operator only 0b00001000 we don't need this

class Server;
class Client;

class Channel
{
	private :
		std::string			_channel_name;
		std::string			_topic;
		time_t				_topic_time;
		std::string			_topic_setter;
		std::string			_topic_setter_source;
		std::set<std::string> _operators;
		std::vector<int>	_fdlist;
		int					_mode;
		std::string			_key;
		int					_limit;
		std::set<std::string> _invited_list;
		Server				*_server;

	public :
		Channel();
		~Channel();
		Channel &operator=(const Channel &other);
		Channel(const Channel &other);

		Channel(std::string myname, Server &server);
		
		std::string	getChannelName() const;
		std::string	getChannelMembers(Channel const &channel, Server &server) const;
		std::string	getChannelTopic() const;
		time_t getTopicTime() const;

		void	setChannelTopic(std::string name);
		bool	isMember(int fd) const;
		int		addClient(Client &client);
		void	removeClient(int fd);
		void	removeOperator(std::string nickname);
		void	removeInvited(std::string nickname);
		bool 	isInvited(std::string nickname);

		void 	setOperator(Client &client, bool enable);
    	bool 	isOperator(const Client &client) const;
    	void 	messageToMembers(Client const &client, std::string cmd, std::string param);
		void	messageToMembersIncludeSelf(Client const &client, std::string cmd, std::string param);
		void	botmessageToMembers(std::string msg) const;

		Server &getServer() const;
		void	setMode(int mode);
		void	unsetMode(int mode);
		bool	isMode(int mode) const;	
		void	setKey(std::string key);
		std::string getKey() const;
		void	setLimit(int limit);
		int		getLimit() const;
		bool	checkInvitedList(Client &client);
		void	addInvitedList(std::string client_name);
		void	setTopicTime(time_t time, std::string topic_setter, std::string source);
		std::string getTopicSetter() const;
		std::string getTopicSetterSource() const;
		std::vector<int> &getFdList();

		void	showChannelMembers(Server &server);  // for Debug
};

# include "../server/Server.hpp"
# include "../client/Client.hpp"

#endif
