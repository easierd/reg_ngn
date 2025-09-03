#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


bool match(Machine *machine, char *string) {
    State *state = machine->state;
    while (*string) {
        if (*string == state->c) {
           state = state->next; 
        } else {
            return false;
        }

        string++;
    }

    if (state == &MATCH_STATE) {
        return true;
    }

    return false;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "usage: match [string]\n");
        return EXIT_FAILURE;
    }

    State a = {'a', NULL};
    State b = {'b', NULL};
    a.next = &b;
    b.next = &MATCH_STATE;
    Machine m = {&a};

    if (match(&m, *++argv)) {
        printf("%s\n", *argv);
    }

    return EXIT_SUCCESS;
}
