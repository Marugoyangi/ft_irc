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

	shutdown(client.getSocket_fd(), SHUT_RDWR);
}