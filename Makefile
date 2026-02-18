CC	?=	clang
CFLAGS	:=	-Wall -Wextra -std=gnu17
CPPFLAGS	:=	-I ./include

SRC	:=	src/main.c \
		src/args.c \
		src/my_ftp.c \
		src/poller.c
OBJ	:=	$(SRC:.c=.o)

BINARY	:=	myftp

all:	$(BINARY)

$(BINARY):	$(OBJ)
	$(CC) -o $(BINARY) $(OBJ)

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(BINARY)

re:	fclean all

.PHONY:	all clean fclean re
