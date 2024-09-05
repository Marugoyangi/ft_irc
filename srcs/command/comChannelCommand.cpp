#include "CommandHandler.hpp"

void CommandHandler::invite(Command &cmd, Client &client, Server &server)
{
    std::string channel_name;
    std::string client_name;
    std::map<std::string, Channel*> &channels = server.getChannels();

    std::vector<std::string> _tem = cmd.getParams();
    if (_tem.size() < 2)
    {
        reply(client, "461", "INVITE", "Not enough parameters");
        return;
    }

    channel_name = _tem[0];
    client_name = _tem[1];

    //존재하는 채널인지 확인
    if (channels.find(channel_name) == channels.end())
    {
        reply(client, "403", channel_name, "No such channel");
        return;
    }

    Channel *channel = channels[channel_name];

    //채널 관리자만 invite
    if (!channel->isOperator(client))
    {
        reply(client, "482", channel_name, "You're not channel operator");
        return;
    }

    // 초대할 사용자 확인
    // 초대할 사용자 확인
    std::map<int, Client> temp = server.getClients();
    Client* target_client = NULL;

    std::map<int, Client>::iterator it;
    for (it = temp.begin(); it != temp.end(); ++it) {
    Client& client = it->second;  // 반복자의 second는 Client 객체
    	if (client.getNickname() == client_name) {
        	target_client = &client;
        	break;  // 조건을 만족하면 반복문 탈출
    	}
    }

    
    if (!target_client)
    {
        reply(client, "401", client_name, "No such nick");
        return;
    }

    // 사용자가 이미 채널에 있는지 확인
    if (channel->isMember(target_client->getSocket_fd()))
    {
        reply(client, "443", client_name, "User already on channel");
        return;
    }

    // 초대 전용 모드인지 확인
    if (!channel->isMode(MODE_I))
    {
        reply(client, "470", channel_name, "Invite only channel");
        return;
    }

    // 초대 처리
    channel->addInvitedList(client_name);
    reply(client, "341", client_name, channel_name);  // 초대 성공 메시지 전송

    // 채널에 초대 정보를 전달
    std::string invite_msg = ":" + client.getNickname() + " INVITE " + client_name + " " + channel_name + "\r\n";
    channel->messageToMembers(client, "INVITE", invite_msg);
}
