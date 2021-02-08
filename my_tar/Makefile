NAME = my_tar

SRCS = *.c

SANITIZE = -g3 -fsanitize=address
CFLAGS = -Wall -Wextra -Werror

all: $(NAME) 

$(NAME) :
	gcc $(SANITIZE) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f *.o

fclean:
	rm -f $(NAME)

re: fclean all