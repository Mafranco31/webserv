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
$(NAME):    $(OBJS) | $(UPL_DIR) $(TMP_DIR)
	$(CC) $(CFLAGS) $^ -o $@
#	@echo "\033[1;32mwebserv ready to launch, use 80 as argument and connect in a browther as localhost.\033[0m"

$(TMP_DIR):
	@mkdir $@

$(OBJ_DIR):
	@mkdir $@

$(UPL_DIR):
	@mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile $(HTML_FILES) $(CGI_FILES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR) $(UPL_DIR)/* $(TMP_DIR)/*

fclean: clean
	$(RM) $(NAME)

re:	clean all

clean_uploads:
	$(RM) $(UPL_DIR)

.PHONY: all clean fclean re
