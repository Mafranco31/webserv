# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By:  dlitran && mafranco && gmacias-           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/01 15:26:30                      #+#    #+#              #
#    Updated: 2024/10/23 11:47:44 by gmacias-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME 		=	webserv
CC			= 	c++
FLAGS		= 	-std=c++98 -Wall -Wextra -Werror -pedantic -g3 -MMD -MP $(FLEAKS)
FLEAKS		=	-fsanitize=address
OS		:=	$(shell uname)

HTML_FILES 	= $(wildcard $(HTML_DIR)/*)
CGI_FILES	= $(wildcard $(CGI_DIR)/*)

###############################################################################
#									SRC										  #
###############################################################################

SRC			=	src/webserv.cpp \
				src/error/ \
				src/network/ \
				src/parse/ \

OBJ		= $(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEPS		= $(addprefix $(DPS_DIR), $(notdir $(SRC:.c=.d)))

HTML_FILES 	= $(wildcard $(HTML_DIR)/*)
CGI_FILES	= $(wildcard $(CGI_DIR)/*)

###############################################################################
#									DIR_PATH								  #
###############################################################################

SRC_DIR		=	src/
OBJ_DIR		=	.obj/
DPS_DIR		=	.dps/
UPL_DIR		=	ploads/
HTML_DIR	=	www/
CGI_DIR		=	cgi-bin/
TMP_DIR		=	tmp/

###############################################################################
#									LIBRARIES								  #
###############################################################################

LIB_PATH	=	libraries/
LIB_FLAGS	=	-L -l

ifeq ($(OS), Linux)
	_PATH	= $(LIB_PATH)
	LIB		= $(_PATH)/.a
	_FLAGS	= -L$(_PATH) -
else
	_PATH	= $(LIB_PATH)
	LIB		= $(_PATH)/.a
	_FLAGS	= -L$(_PATH) -
endif

###############################################################################
#									INLUDES									  #
###############################################################################

INCS	= -I includes/common/

ifeq ($(OS), Linux)
	INCS	+= -I includes/linux/
else
	INCS	+= -I includes/macos/
endif

###############################################################################
#									COLORS									  #
###############################################################################

DEF_COLOR		= 	\033[0m
GREEN 			= 	\033[38;5;46m
WHITE 			= 	\033[38;5;15m
GREY 			= 	\033[38;5;8m
ORANGE 			= 	\033[38;5;202m
RED 			= 	\033[38;5;160m

###############################################################################
#									RULES									  #
###############################################################################

all: make_dir $(NAME)
	@echo "" && echo "$(YELLOW)$(NAME) ready to use:$(DEF_COLOR)"

make_dir:
	@mkdir -p $(OBJ_DIR) $(DPS_DIR) $(UPL_DIR)
	@echo "$(GREEN)Compiling OBJECTS:$(DEF_COLOR)"



$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HTML_FILES) $(CGI_FILES) | make_dir
	@mkdir -p $(dir $@)
	@echo "$(GRAY)Compiling $< to $@ $(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@mv $(basename $@).d $(DPS_DIR)

#					--------	MAKE LIBRARIES	--------						  #


#					--------	RULES PROGRAM	--------						  #

$(NAME):  Makefile $(LIB) $(OBJ) | $(UPL_DIR) $(TMP_DIR)
	@echo "$(MAGENTA)Compiling $(NAME)$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(INCS) $(OBJ) -o $(NAME) 
	@echo "$(BLUE)webserv ready to launch, use 80 as argument and connect in a browther as localhost.$(DEF_COLOR)"

clean:
	@echo "$(RED)Removing ON $(NAME); OBJs and DEPs... $(DEF_COLOR)"
	@/bin/rm -rf $(OBJ_DIR) $(DPS_DIR)
	@echo "$(ORANGE)Done!$(DEF_COLOR)" && echo ""

fclean: clean
	@echo "$(RED)Removing execute $(NAME)... $(DEF_COLOR)"
	@/bin/rm -f $(NAME)
	@echo "$(ORANGE)Done!$(DEF_COLOR)" && echo ""
	
re:	clean all



clean_uploads:
	$(RM) $(UPL_DIR)

###############################################################################
#									OTHERS									  #
###############################################################################

-include $(DEPS)
.PHONY: all clean fclean re		clean_uploads
