#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <iostream>
# include <vector>

class Client;

class Command 
{
	private :
		std::string _command;
		std::string _tag;
		std::string _source;
		std::vector<std::string> _parameter;

	public :
		Command();
		~Command();
		Command &operator=(const Command &other);
		Command(const Command &other);

		void		clearCommand();
		void		parseCommand(std::string command);
		void		execCommand(Client const *clientlist);
		std::string	getCommand();
		std::string	getTag();
		std::string	getMessage();

		void	showCommand();
};

#endif