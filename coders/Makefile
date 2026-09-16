NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS		= main.c parsing.c init.c coder.c dongle.c scheduler.c heap.c logger.c monitor.c utils.c cleanup.c
OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Compiling Codexion..."
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Codexion is ready!"

%.o: %.c structures.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	rm -f $(OBJS)

fclean: clean
	@echo "Cleaning executable..."
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re