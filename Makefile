# ------------------------------------- VARS ----------------------------------

NAME		=	WebServer

SRC			=	./srcs/cgi/CGIHandler.cpp \
				./srcs/config/ConfigParser.cpp ./srcs/config/Parser.cpp \
				./srcs/error/ErrorHandler.cpp \
				./srcs/http/RequestHandler.cpp ./srcs/http/ResponseHandler.cpp \
				./srcs/socket/SocketHandler.cpp \
				./srcs/utils/utils1.cpp \
				main.cpp


INCLUDES	= ./includes/

OBJS		=	$(SRC:.cpp=.o)

CC			=	c++

# -std=c++98 displays an error if an unauthorized function is left in the code
CXXFLAGS		= -std=c++98 -Wall -Wextra -Werror -g3 -fsanitize=address -I ${INCLUDES}

EXE_NAME	=	-o $(NAME)

# ------------------------------------- RULES ---------------------------------

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) $(EXE_NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re:	fclean all

.cpp.o:
	${CC} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

.PHONY:	all clean fclean re