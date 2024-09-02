#include "CommandHandler.hpp"

void CommandHandler::privmsg(Command const &cmd, Client const &client, Server &server)
{
    std::vector<std::string> params = cmd.getParams();

    if (params.empty())
    {
        // ERR_NORECIPIENT
        _reply += ":localhost 411 " + client.getNickname() + " :No recipient given (PRIVMSG)\r\n";
        return;
    }

    if (params.size() < 2)
    {
        // ERR_NOTEXTTOSEND
        _reply += ":localhost 412 " + client.getNickname() + " :No text to send\r\n";
        return;
    }

    std::string target_list = params[0];
    std::string message = params[1];

    std::vector<std::string> targets;
    size_t pos = 0;
    while ((pos = target_list.find(',')) != std::string::npos)
    {
        targets.push_back(target_list.substr(0, pos));
        target_list.erase(0, pos + 1);
    }
    targets.push_back(target_list);

    for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
    {
        std::string target = *it;

        if (target[0] == '#')
        {
            // 채널 메시지 처리
            std::map<std::string, Channel*> &channels = server.getChannels();
            if (channels.find(target) != channels.end())
            {
                Channel &channel = *channels[target];
                channel.messageToMembers(client, "PRIVMSG" + target, message + "\r\n");
            }
            else
            {
                // ERR_NOSUCHCHANNEL
                _reply += ":localhost 403 " + client.getNickname() + " " + target + " :No such channel\r\n";
            }
        }
        else if (target[0] == '#' || target[0] == '$')
		{
			// 호스트 마스크나 서버 마스크에 대한 처리
			std::map<int, Client> &clients = server.getClients();
			bool mask_found = false;

			for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
			{
				Client &recipient = it->second;
				
				// 호스트 마스크 처리: 서버에 연결된 클라이언트의 호스트명이 마스크와 일치하는지 확인
				if (target[0] == '$' && recipient.getHostname().find(target.substr(1)) != std::string::npos)
				{
					mask_found = true;
					std::string full_msg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
					send(recipient.getSocket_fd(), full_msg.c_str(), full_msg.length(), 0);
				}
				
				// 서버 마스크 처리: 서버에 연결된 클라이언트의 서버명이 마스크와 일치하는지 확인
				if (target[0] == '#' && recipient.getServer()->getServerName().find(target.substr(1)) != std::string::npos)
				{
					mask_found = true;
					std::string full_msg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
					send(recipient.getSocket_fd(), full_msg.c_str(), full_msg.length(), 0);
				}
			}
			if (!mask_found)
			{
				// ERR_NOSUCHNICK: 호스트 마스크 또는 서버 마스크가 일치하는 클라이언트를 찾지 못했을 때
				_reply += ":localhost 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n";
			}
		}
        else
        {
            // 개인 메시지 처리
            std::map<int, Client> &clients = server.getClients();
            bool user_found = false;

            for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (it->second.getNickname() == target)
                {
                    user_found = true;
                    Client &recipient = it->second;
                    std::string full_msg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
                    send(recipient.getSocket_fd(), full_msg.c_str(), full_msg.length(), 0);
                    break;
                }
            }

            if (!user_found)
            {
                // ERR_NOSUCHNICK
                _reply += ":localhost 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n";
            }
        }
    }
}
