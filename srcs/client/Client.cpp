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
    _established_time = other._established_time;
    _last_active_time = other._last_active_time;
    _nick = other._nick;
    _hostname = other._hostname;
    _realname = other._realname;
    _server = other._server;
    _mode = other._mode;
    _ip = other._ip;
    _password = other._password;
    _socket_fd = other._socket_fd;
    return (*this);
}

Client::Client(int fd)
{
    _is_registered = false;
    _established_time = time(NULL);
    _last_active_time = time(NULL);
    _nick = "";
    _hostname = "";
    _realname = "";
    _server = "";
    _mode = "";
    _ip = "";
    _password = "";
    _socket_fd = fd;
}

bool Client::isRegist()
{
    return _is_registered;
}

int Client::getFd()
{
    return _socket_fd;
}