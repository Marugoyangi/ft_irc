#include "Server.hpp"

// Orthodox Canonical Form ////////////////////////////////////////////////////
Server::Server(std::string port, std::string password, tm *time_local) : \
                _port(port), _password(password), _time_local(time_local), _server_name("ircserv") {}

Server::~Server() {}

Server &Server::operator=(const Server &other)
{
    if (this != &other) {
        _port = other._port;
        _password = other._password;
        _time_local = other._time_local;
        _server_fd = other._server_fd;
        _event_fd = other._event_fd;
        _clients = other._clients;
        _server_name = other._server_name;
		_channels = other._channels;
    }
    return *this;
}

Server::Server(const Server &other)
{
    *this = other;
}

std::map<int, Client> &Server::getClients()
{
    return _clients;
}

std::map<std::string, Channel> &Server::getChannels()
{
    return _channels;
}

std::set<std::string> Server::getNicknames()
{
    std::set<std::string> nicknames;
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getNickname() == "")
            continue ;
        nicknames.insert(it->second.getNickname());
    }
    return nicknames;
}

std::string Server::getPort()
{
    return _port;
}

std::string Server::getServerName()
{
    return _server_name;
}

time_t Server::getLocalTime()
{
    return mktime(_time_local);
}

void Server::removeClient(int fd)
{
    _clients.erase(fd);
}

int Server::getEventFd()
{
    return _event_fd;
}

void Server::pingClients()
{
    if (_clients.empty())
        return;
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getIs_registered() == false)
        {
            std::cout << "not registered" << std::endl;
            if (time(NULL) - it->second.getLast_active_time() > 30)
            {
                std::cout << "Client " << it->first << " has timed out" << std::endl;
                std::cout << "register failed" << std::endl;
	            close(it->first);
                continue;
            }
        }
        else if (time(NULL) - it->second.getLast_active_time() > 60)
        {
            std::cout << "Client " << it->first << " has timed out" << std::endl;
            std::cout << "ping failed" << std::endl;
            close(it->first);
            continue;
        }
        std::string ping_msg = ":" + _server_name + " PING " + it->second.getNickname() + "\r\n";
        send(it->first, ping_msg.c_str(), ping_msg.length(), 0);
        std::cout << "\033[01m\033[33mmessage to client " << it->first << ": "  << ping_msg << "\033[0m" << std::endl;
    }
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
    Command     cmd;        // 명령어 임시 저장소

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
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); // 타이머 생성
    if (timer_fd == -1) 
    {
        die("timerfd_create");
    }

    // 타이머 설정: 20초 주기로 핑을 보내도록 설정
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 30;    // 처음 30초 후에 핑 시작
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 30; // 30초 간격으로 반복
    timer_spec.it_interval.tv_nsec = 0;
    
    if (timerfd_settime(timer_fd, 0, &timer_spec, NULL) == -1)
    {
        die("timerfd_settime");
    }
    ev.events = EPOLLIN;
    ev.data.fd = timer_fd;
    if (epoll_ctl(_event_fd, EPOLL_CTL_ADD, timer_fd, &ev) == -1)
    {
        die("epoll_ctl: timer_fd");
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
                int optval = 1;
                if (setsockopt(client, SOL_SOCKET, SO_REUSEADDR | SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
                    die("setsockopt");
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
                Client new_client(client, _password, this);
                _clients.insert(std::pair<int, Client>(client, new_client)); // 클라이언트 클래스 추가
            }
            else if (events[i].data.fd == timer_fd)
            {
                uint64_t exp;
                ssize_t s = read(timer_fd, &exp, sizeof(uint64_t));
                if (s == -1)
                {
                    die("read timer_fd");
                }
                pingClients();
            }
            else
            {
                int client = events[i].data.fd;
                buffer.resize(BUFFER_SIZE);
                
                ssize_t bytes_received = recv(client, &buffer[0], BUFFER_SIZE - 1, 0);
                if (bytes_received < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        std::cout << "recv timeout" << std::endl;
                        continue ;
                    }
                    else
                        perror("recv");
                    printf("Client closed connection.\n");
                    epoll_ctl(_event_fd, EPOLL_CTL_DEL, client, NULL);
                    std::map<int, Client>::iterator it = _clients.find(client);
                    if (it != _clients.end()) 
                    {
                        for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); chan_it++)
                        {
                            if (chan_it->second.isMember(it->second.getSocket_fd()))
                            {
                                chan_it->second.removeClient(it->second.getSocket_fd());
                                chan_it->second.removeOperator(it->second.getNickname());
                                chan_it->second.removeInvited(it->second.getNickname());
                                chan_it->second.messageToMembers(it->second, "QUIT", it->second.getDisconnectMessage());
                            }
                        }
                        _clients.erase(it);
                    }
                    close(client);
                    cleanChans();
                    std::cout << "client disconnected called from server read" << std::endl;
                }
                else if (bytes_received == 0)
                {
                    printf("Client closed connection.\n");
                    epoll_ctl(_event_fd, EPOLL_CTL_DEL, client, NULL);
                    std::map<int, Client>::iterator it = _clients.find(client);
                    if (it != _clients.end()) 
                    {
                        for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); chan_it++)
                        {
                            if (chan_it->second.isMember(it->second.getSocket_fd()))
                            {
                                chan_it->second.removeClient(it->second.getSocket_fd());
                                chan_it->second.removeOperator(it->second.getNickname());
                                chan_it->second.removeInvited(it->second.getNickname());
                                chan_it->second.messageToMembers(it->second, "QUIT", it->second.getDisconnectMessage());
                            }
                        }
                        _clients.erase(it);
                    }
                    close(client);
                    cleanChans();
                    std::cout << "client disconnected called from server read" << std::endl;
                }
                else
                {
                    buffer.resize(bytes_received);
                    size_t pos;
                    std::map<int, Client>::iterator it = _clients.find(client);
                    Client &tmp_client = it->second;
                    if (it != _clients.end())
                        tmp_client = it->second;
                    else
                        continue; // 이게 가능한 얘긴가?
                    std::string data = tmp_client.getLeftover() + buffer;
                    tmp_client.setLeftover("");
                    while ((pos = data.find("\r\n")) != std::string::npos || (pos = data.find("\n")) != std::string::npos)
                    {
                        std::string message = data.substr(0, pos);
                        printf("Received message: %s\n", message.c_str());
                        // Process the message ////////////////////////////////
                        cmd.clearCommand();
                        cmd.parseCommand(message);
                        cmd.showCommand();
                        tmp_client.execCommand(cmd, *this);
                        data.erase(0, pos + (data[pos] == '\r' ? 2 : 1));  // Remove the processed message
                    }
                    // Remaining data is saved in leftover
                    tmp_client.setLeftover(data);
                }
            }
        }
    }
    stopEpoll(); // Clean up
    epoll_ctl(_event_fd, EPOLL_CTL_DEL, timer_fd, NULL);
}

void Server::stopEpoll()
{
    epoll_ctl(_event_fd, EPOLL_CTL_DEL, _server_fd, NULL);
    close(_server_fd);
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        int client_fd = it->first;
        epoll_ctl(_event_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    }
    close(_event_fd);
}
#endif


#ifdef __APPLE__
void Server::setupKqueue()
{
    struct kevent change_list;
    struct kevent event_list[MAX_EVENTS];
    std::string buffer;
    std::string leftover;
    Command     cmd;        // 명령어 임시 저장소
    
    setupSocket();

    // kqueue 생성
    _event_fd = kqueue();
    if (_event_fd == -1)
    {
        die("kqueue");
    }

    // 서버 소켓을 kqueue에 등록 (readable 이벤트)
    EV_SET(&change_list, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    if (kevent(_event_fd, &change_list, 1, NULL, 0, NULL) == -1)
    {
        die("kevent: listen_sock");
    }

    // 타이머 설정 (30초 주기로 이벤트 발생)
    EV_SET(&change_list, 0, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 30 * 1000, NULL);
    if (kevent(_event_fd, &change_list, 1, NULL, 0, NULL) == -1)
    {
        die("kevent: timer");
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
                // Set the client socket timeout to 15 seconds
                struct timeval tv = {15, 0};
                if (setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)
                    die("setsockopt");
                // 클라이언트 소켓을 non-blocking으로 설정
                if (fcntl(client, F_SETFL, O_NONBLOCK) == -1)
                    die("fcntl");

                // 새 클라이언트 소켓을 kqueue에 등록 (readable 이벤트)
                EV_SET(&change_list, client, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                if (kevent(_event_fd, &change_list, 1, NULL, 0, NULL) == -1)
                {
                    die("kevent: client");
                }
                Client new_client(client, _password, this);
                _clients.insert(std::pair<int, Client>(client, new_client)); // 클라이언트 클래스 추가
            }
            else if (event_list[i].filter == EVFILT_TIMER)
            {
                // 타이머 이벤트 처리
                pingClients(); // 클라이언트에게 핑 보내기
                // 타이머를 다시 설정할 필요가 없습니다. `EVFILT_TIMER`는 자동으로 재설정됩니다.
            }
            else // 클라이언트 소켓에서 이벤트 발생
            {
                int client = event_list[i].ident;
                buffer.resize(BUFFER_SIZE);

                ssize_t bytes_received = recv(client, &buffer[0], BUFFER_SIZE - 1, 0);
                if (bytes_received < 0)
                {
                    std::string ermsg = strerror(errno);
                    perror("recv");
                    EV_SET(&change_list, client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(_event_fd, &change_list, 1, NULL, 0, NULL);
                    std::map<int, Client>::iterator it = _clients.find(client);
                    if (it != _clients.end()) 
                    {
                        for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); chan_it++)
                        {
                            if (chan_it->second.isMember(it->second.getSocket_fd()))
                            {
                                chan_it->second.removeClient(it->second.getSocket_fd());
                                chan_it->second.removeOperator(it->second.getNickname());
                                chan_it->second.removeInvited(it->second.getNickname());
                                chan_it->second.messageToMembers(it->second, "QUIT", ermsg);
                            }
                        }
                        _clients.erase(it);
                    }
                    close(client);
                    cleanChans();
                }
                else if (bytes_received == 0)
                {                    
                    printf("Client closed connection.\n");
                    EV_SET(&change_list, client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(_event_fd, &change_list, 1, NULL, 0, NULL);
                    std::map<int, Client>::iterator it = _clients.find(client);
                    if (it != _clients.end()) 
                    {
                        for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); chan_it++)
                        {
                            if (chan_it->second.isMember(it->second.getSocket_fd()))
                            {
                                chan_it->second.removeClient(it->second.getSocket_fd());
                                chan_it->second.removeOperator(it->second.getNickname());
                                chan_it->second.removeInvited(it->second.getNickname());
                                chan_it->second.messageToMembers(it->second, "QUIT", it->second.getDisconnectMessage());
                            }
                        }
                        _clients.erase(it);
                    }
                    close(client);
                    cleanChans();
                }
                else
                {
                    buffer.resize(bytes_received);
                    leftover.clear();
                    std::string data = leftover + buffer;
                    
                    size_t pos;
                    std::map<int, Client>::iterator it = _clients.find(client);
                    Client &tmp_client = it->second;
                    if (it != _clients.end())
                        tmp_client = it->second;
                    else
                        continue; // 이게 가능한 얘긴가?
                    while ((pos = data.find("\r\n")) != std::string::npos || (pos = data.find("\n")) != std::string::npos)
                    {
                        std::string message = data.substr(0, pos);
                        printf("Received message  %d: %s\n", client, message.c_str());
                        
                        // Process the message ////////////////////////////////
                        ///////////////////////////////////////////////////////
                        cmd.clearCommand();
                        cmd.parseCommand(message);
                        tmp_client.execCommand(cmd, *this);

						for(std::map<std::string, Channel>::iterator iter = _channels.begin() ; iter != _channels.end(); iter++)
						{
							iter->second.showChannelMembers(*this);
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
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        int client_fd = it->first;
        close(client_fd);
    }
    close(_event_fd); // kqueue는 이벤트fd를 닫으면 자동으로 모든 이벤트 감시를 제거함
}

#endif

void    Server::cleanChans()
{
    std::map<std::string, Channel>::iterator it;
    for (it = _channels.begin(); it != _channels.end();)
    {
        if (it->second.getFdList().size() == 0)
            _channels.erase(it++);
		else
			++it;
    }
}
