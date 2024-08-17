#include "../Server/Server.hpp"

//accept 하자마자 실행됨
//호스트 이름이랑 ip주소랑 비교하는데 이 과정 없어도 작동 잘함.
void firstConnect(int fd)
{
	std::string message;

	message = ":irc.local NOTICE * :*** Looking up your hostname...\r\n";
	message = ":irc.local NOTICE * :*** Could not resolve your hostname: Request timed out; using your IP address (127.0.0.1) instead.\r\n";			
	send(fd, message.c_str(), message.length(), 0);
}
