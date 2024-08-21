NAME = webserv

SRC = srcs/webserv.cpp

CC	=	c++
RM	=	rm -f

FLAGS	=	-std=c++98

all:
	$(CC) $(FLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) uploads/*

fclean: clean
	$(RM) $(NAME)

re: fclean all
