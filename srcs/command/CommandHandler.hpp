#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../server/Server.hpp"
# include "../client/Client.hpp"
# include "../command/Command.hpp"
# include "../channel/Channel.hpp"
# include "../channel/Channel.hpp"

class CommandHandler
{
    private:
        Client  *_client;
        std::string _reply;
    public:
        CommandHandler(Client *client);
        ~CommandHandler();
        CommandHandler(const CommandHandler &other);
        CommandHandler &operator=(const CommandHandler &other);

        void execute(Command &cmd, Client &client, Server &server);

        void reply(int numeric, std::string param, std::string message);
        void reply(std::string const &command, std::vector<std::string> const &message);
        void reply(std::string const &command, std::string const &message);

        // all those commands
        void pass(Command &cmd, Client &client);
        void nick(Command &cmd, Client &client);
        void user(Command &cmd, Client &client);
        void welcome(Client &client);

		void	join(Command &cmd, Client &client, Server &server);
        void    part(Command &cmd, Client &client, std::map<std::string, Channel*> &channels);
        void	names(Command const &cmd, Client const &client, Server &server);
        void    topic(Command const &cmd, Client const &client, Server &server);
		void	privmsg(Command const &cmd, Client const &client, Server &server);
        void    mode(Command const &cmd, Client &client, Server &server);
        //mode 옵션 함수            
        void    handleChannelMode(Channel &channel, Command const &cmd, Client &client);
        void    handleUserMode(Client &client, Command const &cmd);
        void    handleChannelOperatorMode(Channel &channel, Client &client, std::vector<std::string> &params, bool add);

        void    com001(Client const &client, std::string const &server_name);               //RPL_WELCOME 
        void    com461(std::string const &nickname, std::string const &cmd);                //ERR_NEEDMOREPARAMS
        void    com353(Server &server, Channel const *channels);                            //RPL_NAMREPLY
        void    com366(Client const &client, std::string const &channel_name);              //RPL_ENDOFNAMES
        void    com332(Client const &client, Channel const *channel);   //RPL_TOPIC
};
#endif