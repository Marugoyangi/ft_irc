#include "CommandHandler.hpp"

void	CommandHandler::quit(Command &cmd, Client &client, std::map<std::string, Channel> &channels)
{
	std::string msg = client.getNickname();
	std::string send_msg;

	if (cmd.getParams().size() > 0)
		msg = cmd.getParams()[0];
	_reply = "ERROR :Closing Link: (" + client.getUsername() + "@" + client.getHostname() + ") [Quit: " + msg + "]\r\n";	// send_msg = "ERROR :Closing Link: (" + client.getUsername() + "@" + client.getHostname() + ") [Quit: " + msg + "]\r\n";

	(void)channels;
	client.setDisconnectMessage(msg);

	// 클라이언트 정보 삭제
	Server* server = client.getServer();
	int client_fd = client.getSocket_fd();
	close(client_fd);
	#ifdef __linux__
		epoll_ctl(server->getEventFd(), EPOLL_CTL_DEL, client.getSocket_fd(), NULL);
	#endif

	// 서버의 클라이언트 맵에서 클라이언트를 삭제
	server->removeClient(client_fd);
	// 모든 채널에서 클라이언트를 삭제
	for (std::map<std::string, Channel>::iterator it = server->getChannels().begin(); it != server->getChannels().end(); ++it)
	{
    	Channel& channel = it->second;
   		channel.removeClient(client_fd);
    	channel.removeOperator(client.getNickname());
    	channel.removeInvited(client.getNickname());
	}
}