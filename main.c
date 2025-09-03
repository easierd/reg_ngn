#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


bool match(Machine *machine, char *string) {
    for (;*string; string++) {
        machine_step(machine, *string);
        if (machine_broken(machine)) {
            return false;
        }
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

    State branch = {BRANCH, NULL, NULL};
    State a = {'a', NULL, NULL};
    State b = {'b', NULL, NULL};

    branch.next = &a;
    branch.next_2 = &b;

    a.next = &MATCH_STATE;

    b.next = &MATCH_STATE;

    Machine m;
    machine_init(&m, 4, &branch);

    if (match(&m, *++argv)) {
        printf("%s\n", *argv);
    }

    machine_free(&m);

    return EXIT_SUCCESS;
}
