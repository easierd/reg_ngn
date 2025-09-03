.PHONY: clean
CC = gcc
CC_OPTIONS = -Wall -Wextra -fsanitize=address
CC_DEBUG_OPTIONS = -Wall -Wextra -g

match: main.c machine.h
	$(CC) $(CC_OPTIONS) -o match main.c

debug: CC_OPTIONS = $(CC_DEBUG_OPTIONS)
debug: match

clean:
	rm *.o match
