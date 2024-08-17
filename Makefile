NAME = ircserv
SRCS = ./srcs/main.cpp ./srcs/server/Server.cpp ./srcs/command/Command.cpp ./srcs/client/Client.cpp ./srcs/command/CommandHandler.cpp
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