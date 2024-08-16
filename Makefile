NAME = ircserv
SRCS = ./srcs/main.cpp ./srcs/Server/Server.cpp ./srcs/Commands/Command.cpp
OBJS = $(SRCS:.cpp=.o)
C++ = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -I.

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