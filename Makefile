# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/04 16:33:23 by zmogne            #+#    #+#              #
#    Updated: 2025/07/11 14:37:19 by zmogne           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = ircserv

CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

SRCS    = srcs/main.cpp \
          srcs/server/Server.cpp \
		  srcs/server/Message.cpp \
		  srcs/server/Client.cpp \

OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all