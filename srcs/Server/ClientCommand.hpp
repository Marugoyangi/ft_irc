#ifndef CLIENTCOMMAND_HPP
# define CLIENTCOMMAND_HPP

# include <iostream>

class ClientCommand 
{
	private :
		bool		_registered_client;
		std::string _fd;
		std::string	_command;
		std::string _tag;
		std::string _message;

	public :
		ClientCommand(int fd);
		~ClientCommand();
		ClientCommand &operator=(const ClientCommand &other);
		ClientCommand(const ClientCommand &other);
};

#endif