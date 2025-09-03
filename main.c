#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "usage: match [string]\n");
        return EXIT_FAILURE;
    }

    State branch_a_b = {BRANCH, NULL, NULL};
    State branch_ab_c = {BRANCH, NULL, NULL};

    State a = {'a', NULL, NULL};
    State b = {'b', NULL, NULL};
    State c = {'c', NULL, NULL};

    branch_a_b.next = &a;
    branch_a_b.next_2 = &b;

    branch_ab_c.next = &branch_a_b;
    branch_ab_c.next_2 = &c;

    a.next = &MATCH_STATE;
    b.next = &MATCH_STATE;
    c.next = &MATCH_STATE;

    Machine m;
    machine_init(&m, 4, &branch_ab_c);

    if (machine_match(&m, *++argv)) {
        printf("%s\n", *argv);
    }

    machine_free(&m);

    return EXIT_SUCCESS;
}
