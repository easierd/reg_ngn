.PHONY: clean
CC = gcc
CC_OPTIONS = -Wall -Wextra -fsanitize=address
CC_DEBUG_OPTIONS = -Wall -Wextra -g

machine.o: machine.c machine.h
	$(CC) $(CC_OPTIONS) -c machine.c

match: main.c machine.o machine.h
	$(CC) $(CC_OPTIONS) -o match main.c machine.o


debug: CC_OPTIONS = $(CC_DEBUG_OPTIONS)
debug: match

clean:
	rm *.o match
