#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
	private :
		bool		_registered;
		std::string	_nick;
		std::string _ip;
		int			_fd;
	
	public :
		Client();
		~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);

		bool	isRegist();
		int		getFd();
};

#endif