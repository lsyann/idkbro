NAME = codexion
INCLUDE = src/include/
SRCS = $(wildcard src/codexion/*.c)
FLAGS = -Wall -Wextra -Werror
OBJS = $(SRCS:.c=.o)

NB_CODERS = 3
BURNOUT_TIME = 800
COMPILE_TIME = 200
DEBUG_TIME = 200
REFACTOR_TIME = 200
NB_COMPILES = 3
DONGLE_COOLDOWN = 100
SCHEDULER = edf

all: $(NAME)

$(NAME): $(OBJS)
	cc $(FLAGS) $(OBJS) -o $(NAME)

src/codexion/%.o: src/codexion/%.c
	cc $(FLAGS) -I $(INCLUDE) -c $< -o $@
	
run: all
	./$(NAME) $(NB_CODERS) $(BURNOUT_TIME) $(COMPILE_TIME) $(DEBUG_TIME) $(REFACTOR_TIME) $(NB_COMPILES) $(DONGLE_COOLDOWN) $(SCHEDULER)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

rerun: re run
