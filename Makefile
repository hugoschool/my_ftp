CC	?=	clang
CFLAGS	:=	-Wall -Wextra -std=gnu17
CPPFLAGS	:=	-I ./include

ifeq ($(ENV), dev)
	CFLAGS	+=	-g3
endif

SRC	:=	src/main.c \
		src/args.c \
		src/my_ftp.c \
		src/poller.c \
		src/socket.c \
		src/handler.c \
		src/client.c \
		src/client_data.c \

# Commands
SRC	+=	src/commands/handler.c \
		src/commands/quit.c \
		src/commands/user.c \
		src/commands/pass.c \
		src/commands/noop.c \
		src/commands/pwd.c \
		src/commands/cwd.c \
		src/commands/cdup.c \
		src/commands/pasv.c \
		src/commands/list.c \
		src/commands/retr.c \
		src/commands/help.c

# Utils
SRC	+=	src/utils/strcmp_start.c \
		src/utils/strcmp_end.c \
		src/utils/remove_crlf.c

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
