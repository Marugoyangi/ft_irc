#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../server/Server.hpp"
# include "../channel/Channel.hpp"

class CommandHandler;
class Command;
class Server;
class Client
{
	private:
		Server 			*_server;
		bool		_is_registered;
		bool		_is_passed;
		time_t		_established_time;
		time_t		_last_active_time;
		std::string _nickname;
		std::string	_username;
		std::string _realname;
		std::string _hostname;
		std::string _mode;
		std::string _ip;
		std::string _password;
		std::string	_try_password;
		int			_socket_fd;

	
	public:
		Client(int fd, std::string password, Server *server);
		~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);	
		void	execCommand(Command &cmd, Server &server);

		std::string	getSource() const;  //cline의 정보를 source로 만들어주는 함수

		/// Setters and Getters
		void	setTry_password(std::string password);
		void	setNickname(std::string nickname);
		void  	setUsername(std::string username);
		void  	setRealname(std::string realname);
		void  	setHostname(std::string hostname);
		void  	setMode(std::string mode);
		void  	setIp(std::string ip);
		void  	setPassword(std::string password);
		void  	setSocket_fd(int fd);
		void	setIs_registered(bool is_registered);
		void	setIs_passed(bool is_passed);
		void	setEstablished_time(time_t established_time);
		void	setLast_active_time(time_t last_active_time);
		std::string	getNickname() const;
		std::string	getUsername() const;
		std::string	getRealname() const;
		std::string	getHostname() const;
		std::string	getMode() const;
		std::string	getIp() const;
		std::string	getPassword() const;
		std::string	getTry_password() const;
		int			getSocket_fd() const;
		bool		getIs_registered() const;
		bool		getIs_passed() const;
		time_t		getEstablished_time() const;
		time_t		getLast_active_time() const;
		Server		*getServer() const;
		void 		setInvisibleMode(bool enable); // invisible mode 설정
    	bool 		isInvisible() const; // invisible mode 값 확인

		//debug

		void	showClient();

};

#endif