#include "./server/Server.hpp"

// global variable to check if the server is shutting down
volatile sig_atomic_t g_shutdown = false;

void die(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_os()
{
    #ifdef __linux__
        std::cout << "OS: Linux" << std::endl;
    #elif __APPLE__
        std::cout << "OS: MacOS" << std::endl;
    #else
        std::cout << "OS: not supported" << std::endl;
    #endif
}

static void	signal_handler(int signal)
{
	if (g_shutdown == false && signal == SIGTERM)
	{
		//std::cout << "Shutting down server..." << std::endl;
		g_shutdown = true;
	}
}

int main (int argc, char **argv)
{
	if (argc == 3)
	{
        check_os();
		// get current time
		time_t time_raw = {0};
		tm *time_local;

		time(&time_raw);
		time_local = localtime(&time_raw);

		// handle signals
		signal(SIGINT, SIG_IGN);
		signal(SIGTERM, signal_handler);

		// create server
		Server server(argv[1], argv[2], time_local);

		// launch server
        #ifdef __linux__
            server.setupEpoll();
        #endif
        #ifdef __APPLE__
            server.setupKqueue();
        #endif
		return (0);
	}
	else
	{
		std::cout << "input ./ircserv [port] [password]" << std::endl;
		return (1);
	}
}