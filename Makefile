.PHONY: clean
CC = gcc
CC_OPTIONS = -Wall -Wextra -fsanitize=address
CC_DEBUG_OPTIONS = -Wall -Wextra -g

machine.o: machine.c machine.h
	$(CC) $(CC_OPTIONS) -c machine.c

compiler.o: compiler.c compiler.h machine.h compiler_utils.h
	$(CC) $(CC_OPTIONS) -c compiler.c

match: main.c machine.o machine.h compiler.o
	$(CC) $(CC_OPTIONS) -o match main.c machine.o compiler.o


debug: CC_OPTIONS = $(CC_DEBUG_OPTIONS)
debug: match

clean:
	rm *.o match
