NAME = ircserv
SRCS = ./srcs/main.cpp\
       ./srcs/server/Server.cpp\
	   ./srcs/client/Client.cpp\
	   ./srcs/channel/Channel.cpp\
	   ./srcs/command/Command.cpp\
	   ./srcs/command/CommandHandler.cpp\
	   ./srcs/command/comNumberic.cpp\
	   ./srcs/command/comPart.cpp\
	   ./srcs/command/comNames.cpp\
	   ./srcs/command/comJoin.cpp\
	   ./srcs/command/comTopic.cpp
OBJS = $(SRCS:.cpp=.o)
C++ = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -I. -g3

all : $(NAME)

$(NAME) : $(OBJS)
	$(C++) $(FLAGS) -o $(NAME) $(OBJS)

%.o : %.cpp
	$(C++) $(FLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re