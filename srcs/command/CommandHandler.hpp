#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../server/Server.hpp"
# include "../client/Client.hpp"
# include "../command/Command.hpp"

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

        void execute(Command &cmd, Client &client);
        void reply(int numeric, std::string param, std::string message);

        // all those commands
        void pass(Command &cmd, Client &client);
        void nick(Command &cmd, Client &client);
        void user(Command &cmd, Client &client);
        void welcome(Client &client);    
};
#endif