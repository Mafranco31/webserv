SRC_DIR 	= src
SRC 		= $(wildcard $(SRC_DIR)/*.cpp)
NAME 		= webserv
OBJ_DIR 	= .obj
UPL_DIR		= uploads
OBJS 		= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
CC 			= g++
CFLAGS 		= -Wall -Wextra -Werror -std=c++98
DEPFLAGS 	= -MMD -MP
INCLUDE_DIR = inc
INCLUDE 	= $(wildcard $(INCLUDE_DIR)/*.hpp)
RM 			= rm -rf
DEP_DIR 	= dep
DEPS 		= $(OBJS:.o=.d)

all: $(NAME)

-include $(DEPS)
$(NAME):    $(OBJS) | $(UPL_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "\033[1;32mwebserv ready to launch, use 80 as argument and connect in a browther as localhost.\033[0m"

$(OBJ_DIR):
	@mkdir $@

$(UPL_DIR):
	@mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)
	$(RM) uploads/*

fclean: clean
	$(RM) $(NAME)

re:	clean all

clean_uploads:
	$(RM) $(UPL_DIR)

.PHONY: all clean fclean re