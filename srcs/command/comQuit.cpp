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
	Server *server = client.getServer();
	close(client.getSocket_fd());
    std::cout << "Server closed connection." << std::endl;
	#ifdef __APPLE__
		kevent(server->getEventFd(), &change_list, 1, NULL, 0, NULL);
	#endif
	#ifdef __linux__
		epoll_ctl(server->getEventFd(), EPOLL_CTL_DEL, client.getSocket_fd(), NULL);
	#endif
	std::map<int, Client>::iterator it = server->getClients().find(client.getSocket_fd());
	if (it != server->getClients().end()) 
	{
		for (std::map<std::string, Channel>::iterator chan_it = server->getChannels().begin(); chan_it != server->getChannels().end(); chan_it++)
		{
			if (chan_it->second.isMember(it->second.getSocket_fd()))
			{
				chan_it->second.removeClient(it->second.getSocket_fd());
				chan_it->second.removeOperator(it->second.getNickname());
				chan_it->second.removeInvited(it->second.getNickname());
				chan_it->second.messageToMembers(it->second, "QUIT", it->second.getDisconnectMessage());
			}
		}
		server->getClients().erase(it);
	}
	server->cleanChans();
}