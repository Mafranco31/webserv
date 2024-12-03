SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.cpp)
NAME = web_parse
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
DEPFLAGS = -MMD -MP
INCLUDE_DIR = inc
INCLUDE = $(wildcard $(INCLUDE_DIR)/*.hpp)
RM = rm -f
DEP_DIR = dep
DEPS = $(OBJS:.o=.d)

all: $(NAME)

-include $(DEPS)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp Makefile | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS)
	@rmdir $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re:	clean all

.PHONY: all clean fclean re
