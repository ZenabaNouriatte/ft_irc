# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/04 16:33:23 by zmogne            #+#    #+#              #
#    Updated: 2025/07/15 10:19:23 by cschmid          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ================== CONFIGURATION ================== #
NAME      := ircserv
CC        := c++
CFLAGS    := -Wall -Wextra -Werror -std=c++98 -MMD
OPTIONS   := -I includes

SRC_DIR   := srcs
OBJ_DIR   := obj

HEADER_NAMES := Channel.hpp Client.hpp CommandHanler.hpp Message.hpp Server.hpp
SRC_FILES    := main.cpp \
                server/Server.cpp \
                server/Message.cpp \
                server/Client.cpp

SRC := $(addprefix $(SRC_DIR)/, $(SRC_FILES))
HEADER_FILES := $(addprefix includes/, $(HEADER_NAMES))
OBJS := $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS := $(OBJS:.o=.d)

# ================== COULEURS ================== #
GREEN  = \033[32m
BLUE   = \033[34m
RESET  = \033[0m
RED    = \033[31m

# ================== RÈGLES ================== #
all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)Linking objects...$(RESET)"
	@$(CC) $(CFLAGS) $(OPTIONS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ Compilation réussie !$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(OPTIONS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)✓ Objets et dépendances supprimés.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✓ Exécutable supprimé.$(RESET)"

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re