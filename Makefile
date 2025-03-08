FLAGS = -Wall -Wextra -Werror 

CPPC = c++ -std=c++98 -g -fsanitize=address

SOURCES =  main.cpp server.cpp client.cpp handle_cmd.cpp after_auth.cpp Channel.cpp utils.cpp join.cpp mode.cpp operations.cpp invite.cpp topic.cpp kick.cpp
OBJECTS = $(SOURCES:.cpp=.o)

NAME = ircserv
HEADERS = Makefile server.hpp client.hpp Channel.hpp utils.hpp
RM = rm -f
#anssi Colors:
GREEN       = \e[38;5;118m
YELLOW      = \e[38;5;226m
GRAY        = \e[38;5;37m
RESET       = \e[0m

%.o: %.cpp $(HEADERS)
	$(CPPC) $(FLAGS) -c $< -o $@

all : $(NAME)

$(NAME): $(OBJECTS)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\n"
	@$(CPPC) $(FLAGS) $(OBJECTS) -o $(NAME)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

clean:
	@$(RM) $(OBJECTS)
	@printf "$(YELLOW)    - objects removed.$(RESET)\n"
	
fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re : fclean all

.PHONY: clean
