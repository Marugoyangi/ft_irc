#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 512
#define SERVER_IP "119.64.184.89"
#define SERVER_PORT 10007
#define RESPONSE_LENGTH 14 // 서버에서 보내는 응답의 길이

void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        die("socket");
    }

    // 서버 주소 구조체 초기화
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // IP 주소 변환
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        die("inet_pton");
    }

    // 서버에 연결 요청
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        die("connect");
    }

    std::cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    while (true) {
        std::cout << "Enter message to send (type 'exit' to quit): ";
        std::cin.getline(buffer, sizeof(buffer));

        if (strlen(buffer) == 0) {
            std::cout << "Empty message, nothing to send." << std::endl;
            continue;  // 빈 문자열은 서버로 전송하지 않고 루프의 시작으로 돌아감
        }

        // 'exit' 입력 시 종료
        if (std::strcmp(buffer, "exit") == 0) {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        // 메시지 끝에 개행 문자 추가
        std::string message(buffer);
        message += "\r\n";

        // 서버에 메시지 전송
        ssize_t sent_bytes = send(sockfd, message.c_str(), message.size(), 0);
        if (sent_bytes < 0) {
            die("send");
        } else if (sent_bytes != message.size()) {
            std::cerr << "Warning: Not all data was sent." << std::endl;
        }

        // 서버로부터 응답 받기
        char response[RESPONSE_LENGTH];
        ssize_t bytes_received = recv(sockfd, response, sizeof(response), 0);
        if (bytes_received < 0) {
            die("recv");
        } else if (bytes_received == 0) {
            std::cout << "Server closed connection." << std::endl;
            break;
        }

        std::cout << "Server response: " << response << std::endl;
    }

    // 소켓 닫기
    close(sockfd);

    return 0;
}
