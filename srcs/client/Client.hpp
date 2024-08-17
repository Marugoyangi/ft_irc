#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../server/Server.hpp"

class CommandHandler;
class Command;
class Client
{
	private:
		bool		_is_registered;
		bool		_is_passed;
		time_t		_established_time;
		time_t		_last_active_time;
		std::string	_username;
		std::string _realname;
		std::string _hostname;
		std::string _mode;
		std::string _ip;
		std::string _password;
		std::string	_try_password;
		int			_socket_fd;

	
	public:
		Client();					 // it shouldn't be used
		~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);	
		Client(int fd, std::string password);

		void	execCommand(Command &cmd);

		/// Setters and Getters
		void	set_try_password(std::string password);
		void  	set_username(std::string username);
		void  	set_realname(std::string realname);
		void  	set_hostname(std::string hostname);
		void  	set_mode(std::string mode);
		void  	set_ip(std::string ip);
		void  	set_password(std::string password);
		void  	set_socket_fd(int fd);
		void		set_is_registered(bool is_registered);
		void		set_is_passed(bool is_passed);
		void		set_established_time(time_t established_time);
		void		set_last_active_time(time_t last_active_time);
		std::string	get_username();
		std::string	get_realname();
		std::string	get_hostname();
		std::string	get_mode();
		std::string	get_ip();
		std::string	get_password();
		std::string	get_try_password();
		int			get_socket_fd();
		bool		get_is_registered();
		bool		get_is_passed();
		time_t		get_established_time();
		time_t		get_last_active_time();

		//debug

		void	showClient();
};

#endif