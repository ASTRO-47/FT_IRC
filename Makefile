FLAGS =	-Wall -Wextra -Werror 

CPPC =	c++ -std=c++98

SOURCES =  sources/main.cpp  sources/server.cpp sources/client.cpp sources/handle_cmd.cpp sources/after_auth.cpp sources/Channel.cpp sources/broadcast.cpp sources/utils.cpp sources/join.cpp \
		sources/mode.cpp sources/operations.cpp sources/invite.cpp sources/topic.cpp sources/kick.cpp sources/commands.cpp
B_SOURCES =	 BONUS/bot.cpp BONUS/bot_utils.cpp BONUS/jokes_capitals.cpp
OBJECTS = $(SOURCES:.cpp=.o)
B_OBJECTS = $(B_SOURCES:.cpp=.o)

BONUS = bot
NAME = ircserv
HEADERS = Makefile sources/server.hpp sources/client.hpp sources/Channel.hpp sources/utils.hpp sources/replies.hpp
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
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\n"
	@$(CPPC) $(FLAGS) $(OBJECTS) -o $(NAME)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

bonus: $(BONUS)

$(BONUS) : $(B_OBJECTS)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(BONUS)... $(RESET)\n"
	@$(CPPC) $(FLAGS) $(B_OBJECTS) -o $(BONUS)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

clean:
	@$(RM) $(OBJECTS) $(B_OBJECTS)
	@printf "$(YELLOW)    - objects removed.$(RESET)\n"
	
fclean: clean
	@$(RM) $(NAME) $(BONUS)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re : fclean all

.PHONY: clean
