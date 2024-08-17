#include "Client.hpp"

Client::Client()
{
}

Client::~Client() {}

Client::Client(const Client &other)
{
    *this = other;
}

Client &Client::operator=(const Client &other)
{
    if (this == &other)
        return (*this);
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

Client::Client(int fd, std::string password)
{
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

void Client::execCommand(Command &cmd)
{
    CommandHandler handler;
    handler.execute(cmd, *this);
}

//////setters and getters

void Client::set_try_password(std::string password)
{
    _try_password = password;
}

void Client::set_username(std::string username)
{
    _username = username;
}

void Client::set_realname(std::string realname)
{
    _realname = realname;
}

void Client::set_hostname(std::string hostname)
{
    _hostname = hostname;
}

void Client::set_mode(std::string mode)
{
    _mode = mode;
}

void Client::set_ip(std::string ip)
{
    _ip = ip;
}

void Client::set_password(std::string password)
{
    _password = password;
}

void Client::set_socket_fd(int fd)
{
    _socket_fd = fd;
}

std::string Client::get_username() const
{
    return _username;
}

std::string Client::get_realname() const
{
    return _realname;
}

std::string Client::get_hostname() const
{
    return _hostname;
}

std::string Client::get_mode() const
{
    return _mode;
}

std::string Client::get_ip() const
{
    return _ip;
}

std::string Client::get_password() const
{
    return _password;
}

std::string Client::get_try_password() const
{
    return _try_password;
}

int Client::get_socket_fd() const
{
    return _socket_fd;
}

void Client::set_is_registered(bool is_registered)
{
    _is_registered = is_registered;
}

void Client::set_is_passed(bool is_passed)
{
    _is_passed = is_passed;
}

void Client::set_established_time(time_t established_time)
{
    _established_time = established_time;
}

void Client::set_last_active_time(time_t last_active_time)
{
    _last_active_time = last_active_time;
}

bool Client::get_is_registered() const
{
    return _is_registered;
}

bool Client::get_is_passed() const
{
    return _is_passed;
}

time_t Client::get_established_time() const
{
    return _established_time;
}

time_t Client::get_last_active_time() const
{
    return _last_active_time;
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