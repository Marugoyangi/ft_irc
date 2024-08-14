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

#ifdef __APPLE__
#include <sys/event.h>
#endif

#ifdef __linux__
#include <sys/epoll.h>
#endif

extern volatile sig_atomic_t g_shutdown;
extern void die(const char* msg);

#define MAX_EVENTS  64
#define BACKLOG     10
#define BUFFER_SIZE 512

class Server
{
    private:
        std::string		_port;
        std::string		_password;
        tm				*_time_local;
        int             _server_fd;
        int             _event_fd;
        std::set<int>   _clients_fds;

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
};

#endif