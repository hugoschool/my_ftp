CC	?=	clang
CFLAGS	:=	-Wall -Wextra -std=c17
CPPFLAGS	:=	-I ./include

SRC	:=	src/main.c
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
