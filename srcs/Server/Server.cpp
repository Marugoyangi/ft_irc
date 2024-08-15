#include "Server.hpp"

// Orthodox Canonical Form ////////////////////////////////////////////////////
Server::Server(std::string port, std::string password, tm *time_local) : \
                _port(port), _password(password), _time_local(time_local) {}

Server::~Server() {}

Server &Server::operator=(const Server &other)
{
    if (this != &other) {
        _port = other._port;
        _password = other._password;
        _time_local = other._time_local;
        _server_fd = other._server_fd;
        _event_fd = other._event_fd;
        _clients_fds = other._clients_fds;
    }
    return *this;
}

Server::Server(const Server &other)
{
    *this = other;
}
////////////////////////////////////////////////////////////////////////////////
// Member Functions ////////////////////////////////////////////////////////////
void Server::setupSocket()
{
    struct addrinfo hints;
    struct addrinfo *res;
    int status;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    // TCP IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // get address info
    if ((status = getaddrinfo(NULL, _port.c_str(), &hints, &res)) != 0)
        die("getaddrinfo");
    // create socket
    _server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_server_fd == -1)
        die("socket");
    // set socket options to reuse address and port
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        die("setsockopt");
    // bind socket to port
    if (bind(_server_fd, res->ai_addr, res->ai_addrlen) == -1)
        die("bind");
    // free the linked list
    freeaddrinfo(res);
    //start listening
    if (listen(_server_fd, 10) == -1)
        die("listen");
    //set non-blocking
    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
        die("fcntl");
}
#ifdef __linux__
void Server::setupEpoll()
{
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    std::string buffer;
    std::string leftover;

    setupSocket();
    if ((_event_fd = epoll_create1(0)) == -1) 
    {
         die("epoll_create1");
    }
    // readable | edge-triggered
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = _server_fd;
    if (epoll_ctl(_event_fd, EPOLL_CTL_ADD, _server_fd, &ev) == -1) 
    {
        die("epoll_ctl: listen_sock");
    }
    while (g_shutdown == false)
    {
        int n = epoll_wait(_event_fd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            die("epoll_wait");
        }

        for (int i = 0; i < n; ++i)
        {
            if (events[i].data.fd == _server_fd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                int client = accept(_server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client < 0)
                {
                    perror("accept");
                    continue;
                }

                // Set the client socket to non-blocking mode
                if (fcntl(client, F_SETFL, O_NONBLOCK) == -1)
                    die("fcntl");
                // Add the new client socket to the epoll instance
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client;
                if (epoll_ctl(_event_fd, EPOLL_CTL_ADD, client, &ev) == -1)
                {
                    die("epoll_ctl: client");
                }
                _clients_fds.insert(client); // FD 추가
            }
            else
            {
                int client = events[i].data.fd;
                buffer.resize(BUFFER_SIZE);
                
                ssize_t bytes_received = recv(client, &buffer[0], BUFFER_SIZE - 1, 0);
                if (bytes_received < 0)
                {
                    perror("recv");
                    epoll_ctl(_event_fd, EPOLL_CTL_DEL, client, NULL);
                    close(client);
                    _clients_fds.erase(client);
                }
                else if (bytes_received == 0)
                {
                    printf("Client closed connection.\n");
                    epoll_ctl(_event_fd, EPOLL_CTL_DEL, client, NULL);
                    close(client);
                    _clients_fds.erase(client);
                }
                else
                {
                    buffer.resize(bytes_received);
                    std::string data = leftover + buffer;
                    leftover.clear();

                    size_t pos;
                    while ((pos = data.find("\r\n")) != std::string::npos || (pos = data.find("\n")) != std::string::npos)
                    {
                        std::string message = data.substr(0, pos);
                        printf("Received message: %s\n", message.c_str());

                        ssize_t sent_bytes = send(client, message.c_str(), message.size(), 0);
                        printf("Sent %ld bytes\n", sent_bytes);
                        if (sent_bytes < 0)
                        {
                            perror("send");
                        }
                        else if (sent_bytes != static_cast<ssize_t>(message.size()))
                        {
                            fprintf(stderr, "Warning: Not all data was sent.\n");
                        }

                        data.erase(0, pos + (data[pos] == '\r' ? 2 : 1));  // Remove the processed message
                    }
                    // Remaining data is saved in leftover
                    leftover = data;
                }
            }
        }
    }
    stopEpoll(); // Clean up
}

void Server::stopEpoll()
{
    epoll_ctl(_event_fd, EPOLL_CTL_DEL, _server_fd, NULL);
    close(_server_fd);
    std::set<int>::iterator it;
    for (it = _clients_fds.begin(); it != _clients_fds.end(); ++it) {
        int client_fd = *it;
         epoll_ctl(_event_fd, EPOLL_CTL_DEL, client_fd, NULL);
         close(client_fd);
    }
    close(_event_fd);
}
#endif


#ifdef __APPLE__
#ifdef __APPLE__
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

void Server::setupKqueue()
{
    struct kevent change_list[MAX_EVENTS];
    struct kevent event_list[MAX_EVENTS];
    std::string buffer;
    std::string leftover;

    setupSocket();

    // kqueue 생성
    _event_fd = kqueue();
    if (_event_fd == -1)
    {
        die("kqueue");
    }

    // 서버 소켓을 kqueue에 등록 (readable 이벤트)
    EV_SET(&change_list[0], _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    if (kevent(_event_fd, change_list, 1, NULL, 0, NULL) == -1)
    {
        die("kevent: listen_sock");
    }

    while (g_shutdown == false)
    {
        int n = kevent(_event_fd, NULL, 0, event_list, MAX_EVENTS, NULL);
        if (n == -1)
        {
            die("kevent");
        }

        for (int i = 0; i < n; ++i)
        {
            if (event_list[i].ident == (unsigned int)_server_fd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                int client = accept(_server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client < 0)
                {
                    perror("accept");
                    continue;
                }

                // 클라이언트 소켓을 non-blocking으로 설정
                if (fcntl(client, F_SETFL, O_NONBLOCK) == -1)
                    die("fcntl");

                // 새 클라이언트 소켓을 kqueue에 등록 (readable 이벤트)
                EV_SET(&change_list[0], client, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                if (kevent(_event_fd, change_list, 1, NULL, 0, NULL) == -1)
                {
                    die("kevent: client");
                }
                _clients_fds.insert(client); // FD 추가
            }
            else
            {
                int client = event_list[i].ident;
                buffer.resize(BUFFER_SIZE);

                ssize_t bytes_received = recv(client, &buffer[0], BUFFER_SIZE - 1, 0);
                if (bytes_received < 0)
                {
                    perror("recv");
                    EV_SET(&change_list[0], client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(_event_fd, change_list, 1, NULL, 0, NULL);
                    close(client);
                    _clients_fds.erase(client);
                }
                else if (bytes_received == 0)
                {
                    printf("Client closed connection.\n");
                    EV_SET(&change_list[0], client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(_event_fd, change_list, 1, NULL, 0, NULL);
                    close(client);
                    _clients_fds.erase(client);
                }
                else
                {
                    buffer.resize(bytes_received);
                    std::string data = leftover + buffer;
                    leftover.clear();
                    
                    size_t pos;
                    while ((pos = data.find("\r\n")) != std::string::npos || (pos = data.find("\n")) != std::string::npos)
                    {
                        std::string message = data.substr(0, pos);
                        printf("Received message: %s\n", message.c_str());

                        ssize_t sent_bytes = send(client, message.c_str(), message.size(), 0);
                        printf("Sent %ld bytes\n", sent_bytes);
                        if (sent_bytes < 0)
                        {
                            perror("send");
                        }
                        else if (sent_bytes != static_cast<ssize_t>(message.size()))
                        {
                            fprintf(stderr, "Warning: Not all data was sent.\n");
                        }

                        data.erase(0, pos + (data[pos] == '\r' ? 2 : 1));  // Remove the processed message
                    }
                    // 남은 데이터를 leftover에 저장
                    leftover = data;
                }
            }
        }
    }
    stopKqueue(); // Clean up
}

void Server::stopKqueue()
{
    close(_server_fd);
    std::set<int>::iterator it;
    for (it = _clients_fds.begin(); it != _clients_fds.end(); ++it) {
        int client_fd = *it;
        close(client_fd);
    }
    close(_event_fd);
}
#endif
