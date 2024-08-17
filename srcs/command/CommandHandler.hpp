#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../server/Server.hpp"
# include "../client/Client.hpp"
# include "../command/Command.hpp"

class CommandHandler
{
    public:
        CommandHandler();
        ~CommandHandler();
        CommandHandler(const CommandHandler &other);
        CommandHandler &operator=(const CommandHandler &other);

        void execute(Command &cmd, Client &client);
        void reply(int numeric, int fd, std::string message);
};
#endif