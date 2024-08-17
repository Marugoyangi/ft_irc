#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <ctime>
# include <../command/Command.hpp>

class Command;
class Client
{
	private :
		bool		_is_registered;
		time_t		_established_time;
		time_t		_last_active_time;
		std::string	_nick;
		std::string _hostname;
		std::string _realname;
		std::string _server;
		std::string _mode;
		std::string _ip;
		std::string _password;
		int			_socket_fd;

	
	public :
		Client();					 // it shouldn't be used
		~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);	
		Client(int fd);

		bool	isRegist();
		int		getFd();
		void	execCommand(Command cmd);
};

#endif