NAME 		= webserv

SRC_DIR 	= src
OBJ_DIR 	= .obj
UPL_DIR		= uploads
HTML_DIR	= www
CGI_DIR		= cgi-bin
INCLUDE_DIR = inc
TMP_DIR		= tmp

HTML_FILES 	= $(wildcard $(HTML_DIR)/*)
CGI_FILES	= $(wildcard $(CGI_DIR)/*)
INCLUDE 	= $(wildcard $(INCLUDE_DIR)/*.hpp)

SRC 		= $(wildcard $(SRC_DIR)/*.cpp)
OBJS 		= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

CC 			= g++
CFLAGS 		= -Wall -Wextra -Werror -std=c++98
DEPFLAGS 	= -MMD -MP

RM 			= rm -rf
DEP_DIR 	= dep
DEPS 		= $(OBJS:.o=.d)

all: $(NAME)

-include $(DEPS)
$(NAME):    $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@


$(OBJ_DIR):
	@mkdir $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile $(HTML_FILES) $(CGI_FILES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re:	clean all

clean_uploads:
	$(RM) $(UPL_DIR)

.PHONY: all clean fclean re
