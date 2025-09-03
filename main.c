#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


bool match(Machine *machine, char *string) {
    while (*string) {
        machine_step(machine, *string);

        if (machine_broken(machine)) {
            return false;
        }

        string++;
    }

    if (machine_has_match(machine)) {
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
    Machine m;
    machine_init(&m, 2, &a);

    if (match(&m, *++argv)) {
        printf("%s\n", *argv);
    }

    machine_free(&m);

    return EXIT_SUCCESS;
}
