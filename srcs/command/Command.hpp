#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "../server/Server.hpp"

class Client;
class Command 
{
	private:
		int			_numeric;
		std::string _tag;
		std::string _source;
		std::string _command;
		std::vector<std::string> _parameter;

	public:
		Command();
		~Command();
		Command &operator=(const Command &other);
		Command(const Command &other);

		void		clearCommand();
		void		parseCommand(std::string command);
		void		setCommand(std::string tag, std::string source, std::string command, std::vector<std::string> parameter);
		std::string	deparseCommand() const;
		std::string	getCommand() const;
		std::string	getTag() const;
		std::string	getMessage() const;
		std::vector<std::string>	getParams() const;

		void	showCommand();
};

#endif