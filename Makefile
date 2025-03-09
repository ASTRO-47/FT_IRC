# FLAGS = -Wall -Wextra -Werror 

CPPC = c++ -std=c++98 -g -fsanitize=address

SOURCES =  main.cpp server.cpp client.cpp handle_cmd.cpp after_auth.cpp Channel.cpp broadcast.cpp
B_SOURCES =	 BONUS/bot.cpp
OBJECTS = $(SOURCES:.cpp=.o)
B_OBJECTS = $(B_SOURCES:.cpp=.o)

BONUS = bot
NAME = ircserv
HEADERS = Makefile server.hpp client.hpp Channel.hpp
B_HEADERS = BONUS/bot.hpp
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

bonus: $(BONUS)

$(BONUS) : $(B_OBJECTS)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(BONUS)... $(RESET)\r\n"
	@$(CPPC) $(FLAGS) $(B_OBJECTS) -o $(BONUS)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

clean:
	@$(RM) $(OBJECTS) $(B_OBJECTS)
	@printf "$(YELLOW)    - objects removed.$(RESET)\r\n"
	
fclean: clean
	@$(RM) $(NAME) $(BONUS)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\r\n"

re : fclean all

.PHONY: clean
