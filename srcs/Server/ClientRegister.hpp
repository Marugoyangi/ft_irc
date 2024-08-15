#ifndef CLIENTREGISTER_HPP
# define CLIENTREGISTER_HPP

# include <iostream>
# include "Commands.hpp"

class ClientRegister
{
	private :
		bool		_registered;
		std::string	_nick;
		std::string _ip;
		Commands	_commands;
		int			_fd;
	
	public :
		ClientRegister();
		~ClientRegister();
		ClientRegister(const ClientRegister &other);
		ClientRegister &operator=(const ClientRegister &other);
};

#endif