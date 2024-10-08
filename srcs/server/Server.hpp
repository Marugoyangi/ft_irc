#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <csignal>
#include <exception>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <algorithm>
#include <cerrno>
#include <sstream>
#include <iterator>
#include "../command/Command.hpp"
#include "../client/Client.hpp"
#include "../command/CommandHandler.hpp"
#include "../channel/Channel.hpp"


#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#endif

#ifdef __linux__
#include <sys/epoll.h>
#include <sys/timerfd.h>
#endif

extern volatile sig_atomic_t g_shutdown;
extern void die(const char *msg);

#define MAX_EVENTS 64
#define BACKLOG 10
#define BUFFER_SIZE 512
#define MAX_CLIENTS 1024
#define MAX_TARGETS 4
#define MAX_FILE_SIZE_MB 10
#define MAX_FILE_SIZE 1024 * 1024 * MAX_FILE_SIZE_MB // 10MB
class Client;
class Server
{
private:
    std::string _port;
    std::string _password;
    tm *_time_local;
    int _server_fd;
    int _event_fd;
    std::string _server_name;
    std::map<int, Client> _clients;
    std::map<std::string, Channel> _channels;

    // std::set<ClientCommand> _clients;

public:
    // Orthodox Canonical Form
    Server(std::string port, std::string password, tm *time_local);
    ~Server();
    Server &operator=(const Server &other);
    Server(const Server &other);
    // Server setup
    void setupSocket();

#ifdef __linux__
    void setupEpoll();
    void stopEpoll();
#endif
#ifdef __APPLE__
    void setupKqueue();
    void stopKqueue();
#endif
    // getter and setter
    std::map<int, Client>           &getClients();
    std::map<std::string, Channel>	&getChannels();
    std::set<std::string>           getNicknames();
    std::string                     getPort();
    std::string                     getServerName();
    time_t                          getLocalTime();
    int                             getEventFd();

    void    cleanChans();
    void    removeClient(int fd);
    void                            pingClients();
};

#endif