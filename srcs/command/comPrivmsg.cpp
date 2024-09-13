#include "CommandHandler.hpp"

void CommandHandler::privmsg(Command const &cmd, Client const &client, Server &server)
{
    std::vector<std::string> params = cmd.getParams();

    if (params.empty())
    {
        // ERR_NORECIPIENT
        reply(411, client.getNickname(), "No recipient given (PRIVMSG)");
        return;
    }

    if (params.size() < 2)
    {
        // ERR_NOTEXTTOSEND
        reply(412, client.getNickname(), "No text to send");
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
            std::map<std::string, Channel> &channels = server.getChannels();
            if (channels.find(target) != channels.end())
            {
                Channel &channel = channels[target];
                channel.messageToMembers(client, "PRIVMSG " + target, message + "\r\n");
                if (message == "!bot")
                    botHelp(client, channels[target]);
                else if (message == "!time")
                    botTime(client, channels[target]);
                else if (message == "!uptime")
                    botUptime(client, channels[target]);
            }
            else
            {
                // ERR_NOSUCHCHANNEL
                reply(403, client.getNickname(), target + " :No such channel");
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
                reply(401, client.getNickname(), target + " :No such nick/channel");
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
                    std::string trimmed_message = message;
                    if (!trimmed_message.empty() && trimmed_message[0] == '\x01') {
                        trimmed_message.erase(0, 1); // ASCII 코드 1 제거
                    }
                    // 메시지의 마지막 부분이 DCC로 시작하는지 확인                    
                    if (trimmed_message.size() > 4 && trimmed_message.substr(0, 4) == "DCC ")
                    {
                        std::cout << "DCC Message: " << trimmed_message << std::endl;
                        if (!handleDCCCommand(trimmed_message, client, server))
                            return;
                    }
                    else
                        std::cout << "not DCC Message: " << trimmed_message << std::endl;
                    std::string full_msg = client.getSource() + " PRIVMSG " + target + " :" + trimmed_message + "\r\n";
                    std::cout << "Full message: " << full_msg << std::endl;
                    send(recipient.getSocket_fd(), full_msg.c_str(), full_msg.length(), 0);
                    break;
                }
            }
            if (!user_found)
            {
                // ERR_NOSUCHNICK
                reply(401, client.getNickname(), target + " :No such nick/channel");
            }
        }
    }
}

bool CommandHandler::handleDCCCommand(std::string const &message, Client const &client, Server &server)
{
    std::vector<std::string> params;
    std::string::size_type start = 0;
    std::string::size_type end = message.find(" ");
    (void)server;

    while (end != std::string::npos) {
        params.push_back(message.substr(start, end - start));
        start = end + 1;
        end = message.find(" ", start);
    }
    params.push_back(message.substr(start));
    if (params.size() < 6)
    {
        return false;
    }
    std::cout << "DCC command: " << params[0] << " " << params[1] << std::endl;
    if (params[1] == "SEND")
    {
        std::string file_size_str = params[5];
        size_t file_size;
        std:: stringstream ss(file_size_str);
        ss >> file_size;
        // 파일 크기 제한 설정 (예: 10MB)
        if (file_size > MAX_FILE_SIZE)
        {
            // 파일 크기 초과 에러 메시지 전송 (PRIVMSG 사용)
            ss << "File size exceeds the limit: " << MAX_FILE_SIZE_MB << "MB";
            reply(411, client.getNickname(), ss.str());
            return false;
        }
    }
    return true;
}