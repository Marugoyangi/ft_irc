#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../server/Server.hpp"
# include "../client/Client.hpp"
# include "../channel/Channel.hpp"

class Command;
class CommandHandler
{
    private:
        Client  *_client;
        std::string _reply;
    public:
        CommandHandler();
        CommandHandler(Client *client);
        ~CommandHandler();
        CommandHandler(const CommandHandler &other);
        CommandHandler &operator=(const CommandHandler &other);

        void execute(Command &cmd, Client &client, Server &server);
        void reply(int numeric, std::string param, std::string message);
        void reply(std::string const &command, std::vector<std::string> const &message);
        void reply(std::string const &command, std::string const &message);
	    void reply(Client &client, const char* numeric, const std::string &channel_name, const std::string &message);
        void setReply(std::string const &reply);
        std::string getReply() const;
        void addReply(std::string const &reply);
        // all those commands
        void pass(Command &cmd, Client &client);
        void nick(Command &cmd, Client &client);
        void user(Command &cmd, Client &client);
        void welcome(Client &client);

		void	join(Command &cmd, Client &client, Server &server);
        void    part(Command &cmd, Client &client, std::map<std::string, Channel> &channels);
        void    kick(Command &cmd, Client &client, Server &server);
        void    quit(Command &cmd, Client &client, std::map<std::string, Channel> &channels);
        void	names(Command const &cmd, Client const &client, Server &server);
		void	privmsg(Command const &cmd, Client const &client, Server &server);
        bool    handleDCCCommand(std::string const &message, Client const &client, Server &server);
        void    mode(Command const &cmd, Client &client, Server &server);
        //mode 옵션 함수            
        void    handleChannelMode(Channel &channel, Command const &cmd, Client &client);
        void    handleUserMode(Client &client, Command const &cmd);
        void    handleChannelOperatorMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);
        void    botHelp(Client const &client, Channel const &channel);
        void    botTime(Client const &client, Channel const &channel);
        void    botUptime(Client const &client, Channel const &channel);

	void	handleChannelInviteMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);
    void    handleChannelKeyMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);
    void    handleChannelLimitMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);
    void    handleChannelTopicMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);
	void 	invite(Command &cmd, Client &client, Server &server);
    void    topic(Command const &cmd, Client const &client, Server &server);
        void    com353(Server &server, Channel &channels);                            //RPL_NAMREPLY
        void    com366(Client const &client, std::string const &channel_name);              //RPL_ENDOFNAMES
        void    com332(std::string topic, Channel &channel, Client &client); //RPL_TOPIC
};
#endif
