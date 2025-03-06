# FLAGS = -Wall -Wextra -Werror 

CPPC = c++ -std=c++98 -g -fsanitize=address

SOURCES =  main.cpp server.cpp client.cpp handle_cmd.cpp after_auth.cpp Channel.cpp
OBJECTS = $(SOURCES:.cpp=.o)

NAME = ircserv
HEADERS = Makefile server.hpp client.hpp Channel.hpp
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
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\r\n"
	@$(CPPC) $(FLAGS) $(OBJECTS) -o $(NAME)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

clean:
	@$(RM) $(OBJECTS)
	@printf "$(YELLOW)    - objects removed.$(RESET)\r\n"
	
fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\r\n"

re : fclean all

.PHONY: clean
