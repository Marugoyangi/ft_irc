#include "Client.hpp"

Client::~Client() {}

Client::Client(const Client &other)
{
    (*this)._server = other._server;
    *this = other;
}

Client &Client::operator=(const Client &other)
{
    if (this == &other)
        return (*this);
    _server = other._server;
    _is_registered = other._is_registered;
    _is_passed = other._is_passed;
    _established_time = other._established_time;
    _last_active_time = other._last_active_time;
    _username = other._username;
    _hostname = other._hostname;
    _realname = other._realname;
    _mode = other._mode;
    _ip = other._ip;
    _password = other._password;
    _socket_fd = other._socket_fd;
    return (*this);
}

Client::Client(int fd, std::string password, Server *server)
{
    _server = server;
    _is_registered = false;
    _is_passed = false;
    _established_time = time(NULL);
    _last_active_time = time(NULL);
    _username = "";
    _hostname = "";
    _realname = "";
    _mode = "";
    _ip = "";
    _password = password;
    _try_password = "";
    _socket_fd = fd;
}

void Client::execCommand(Command &cmd, Server &server)
{
    CommandHandler handler(this);
    handler.execute(cmd, *this, server);
}

std::string	Client::getSource() const
{
	std::string source = ":" + _nickname + "!" + _username + "@" + _hostname;
	return (source);
}

//////setters and getters

void Client::setTry_password(std::string password)
{
    _try_password = password;
}

void Client::setNickname(std::string nickname)
{
    _nickname = nickname;
}

void Client::setUsername(std::string username)
{
    _username = username;
}

void Client::setRealname(std::string realname)
{
    _realname = realname;
}

void Client::setHostname(std::string hostname)
{
    _hostname = hostname;
}

void Client::setMode(std::string mode)
{
    _mode = mode;
}

void Client::setIp(std::string ip)
{
    _ip = ip;
}

void Client::setPassword(std::string password)
{
    _password = password;
}

void Client::setSocket_fd(int fd)
{
    _socket_fd = fd;
}

void Client::setIs_registered(bool is_registered)
{
    _is_registered = is_registered;
}

void Client::setIs_passed(bool is_passed)
{
    _is_passed = is_passed;
}

void Client::setEstablished_time(time_t established_time)
{
    _established_time = established_time;
}

void Client::setLast_active_time(time_t last_active_time)
{
    _last_active_time = last_active_time;
}

std::string Client::getNickname() const
{
    return _nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

std::string Client::getRealname() const
{
    return _realname;
}

std::string Client::getHostname() const
{
    return _hostname;
}

std::string Client::getMode() const
{
    return _mode;
}

std::string Client::getIp() const
{
    return _ip;
}

std::string Client::getPassword() const
{
    return _password;
}

std::string Client::getTry_password() const
{
    return _try_password;
}

int Client::getSocket_fd() const
{
    return _socket_fd;
}

bool Client::getIs_registered() const
{
    return _is_registered;
}

bool Client::getIs_passed() const
{
    return _is_passed;
}

time_t Client::getEstablished_time() const
{
    return _established_time;
}

time_t Client::getLast_active_time() const
{
    return _last_active_time;
}

void Client::setInvisibleMode(bool enable)
{
    if (enable)
        _mode += 'i';
    else
        _mode.erase(std::remove(_mode.begin(), _mode.end(), 'i'), _mode.end());
}

//debug

void Client::showClient()
{
    std::cout << std::endl;
    std::cout << "----Client Status-------------------" << std::endl;
    std::cout << "is_registered: " << _is_registered << std::endl;
    std::cout << "is_passed: " << _is_passed << std::endl;
    std::cout << "established_time: " << _established_time << std::endl;
    std::cout << "last_active_time: " << _last_active_time << std::endl;
    std::cout << "nickname: " << _nickname << std::endl;
    std::cout << "username: " << _username << std::endl;
    std::cout << "hostname: " << _hostname << std::endl;
    std::cout << "realname: " << _realname << std::endl;
    std::cout << "mode: " << _mode << std::endl;
    std::cout << "ip: " << _ip << std::endl;
    std::cout << "password: " << _password << std::endl;
    std::cout << "try_password: " << _try_password << std::endl;
    std::cout << "socket_fd: " << _socket_fd << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

Server *Client::getServer() const
{
    return _server;
}