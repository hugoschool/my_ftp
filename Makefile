CC	?=	clang
CFLAGS	:=	-Wall -Wextra -std=gnu17
CPPFLAGS	:=	-I ./include

SRC	:=	src/main.c \
		src/args.c \
		src/my_ftp.c \
		src/poller.c \
		src/socket.c \
		src/handler.c \
		src/client.c \
		src/client_data.c
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
