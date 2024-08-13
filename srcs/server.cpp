#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>

#define SERVER_PORT 10007
#define MAX_EVENTS 10
#define BACKLOG 10

static int listener;
static int kq;

static void die(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static void setnonblocking(int sock)
{
    int flag = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flag | O_NONBLOCK);
}

static int setup_socket()
{
    int sock;
    int opt = 1;
    struct sockaddr_in sin;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    {
        die("socket");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        die("setsockopt");
    }

    memset(&sin, 0, sizeof sin);
    
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(SERVER_PORT);

    if (bind(sock, (struct sockaddr *) &sin, sizeof sin) < 0)
    {
        close(sock);
        die("bind");
    }

    if (listen(sock, BACKLOG) < 0)
    {
        close(sock);
        die("listen");
    }

    return sock;
}

int main()
{
    struct kevent ev;
    struct kevent events[MAX_EVENTS];
    char buffer[1024];
    char leftover[1024] = {0};  // 이전 수신 데이터의 남은 부분을 저장할 버퍼

    if ((kq = kqueue()) < 0)
    {
        die("kqueue");
    }


    listener = setup_socket();
    setnonblocking(listener);
    EV_SET(&ev, listener, EVFILT_READ, EV_ADD, 0, 0, NULL); // EVFILT_READ: 읽기
    if (kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
    {
        die("kevent");
    }

    for (;;) {
        int i;
        int nfd = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);

        for (i = 0; i < nfd; i++)
        {
            if (events[i].ident == (uintptr_t)listener)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof client_addr;

                int client = accept(listener, (struct sockaddr *) &client_addr, &client_addr_len);
                if (client < 0)
                {
                    perror("accept");
                    continue;
                }
                setnonblocking(client);
                printf("Accepted connection from client.\n");
                EV_SET(&ev, client, EVFILT_READ, EV_ADD, 0, 0, NULL); 
                if (kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
                {
                    perror("kevent");
                }
            }
            else 
            {
                int client = (int)events[i].ident;
                ssize_t n = recv(client, buffer, sizeof buffer - 1, 0);
                if (n < 0)
                {
                    perror("recv");
                    EV_SET(&ev, client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &ev, 1, NULL, 0, NULL);
                    close(client);
                }
                else if (n == 0)
                {
                    printf("Client closed connection.\n");
                    EV_SET(&ev, client, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &ev, 1, NULL, 0, NULL);
                    close(client);
                }
                else
                {
                    buffer[n] = '\0'; // 문자열 종료
                    printf("Received message: %s\n", buffer);
                    char *start = buffer;
                    char *end;

                    // 이전 남은 데이터를 현재 수신된 데이터에 결합
                    if (leftover[0] != '\0') {
                        strncat(leftover, buffer, sizeof(leftover) - strlen(leftover) - 1);
                        start = leftover;
                    }

                    // `\r`, `\n`, `\r\n`을 기준으로 메시지를 분리하여 처리
                    while ((end = strpbrk(start, "\r\n")) != NULL) {
                        *end = '\0';  // 메시지 끝에 null 종료 추가
                        // 클라이언트에 메시지 에코
                        printf("Sending message to client: %s\n", start);
                        ssize_t sent_bytes = send(client, start, strlen(start), 0);
                        printf("Sent %ld bytes\n", sent_bytes);
                        if (sent_bytes < 0)
                        {
                            perror("send");
                        }
                        else if (sent_bytes != strlen(start))
                        {
                            fprintf(stderr, "Warning: Not all data was sent.\n");
                        }

                        // 연속된 \r\n, \n\r 등 처리
                        while (*end == '\r' || *end == '\n') end++;
                        start = end;
                    }

                    // 남은 데이터를 leftover에 저장
                    if (*start != '\0') {
                        strncpy(leftover, start, sizeof(leftover) - 1);
                    } else {
                        leftover[0] = '\0';  // 남은 데이터가 없으면 leftover 비우기
                    }
                }
            }
        }
    }
    return 0;
}
