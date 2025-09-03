#include<assert.h>

#include "../machine.h"

int main(int argc, char *argv[]) {
    State a = {'a', NULL, NULL};
    State branch = {BRANCH, NULL, NULL};

    a.next = &branch;
    branch.next = &MATCH_STATE;
    branch.next_2 = &a;

    Machine m;
    machine_init(&m, 2, &a);

    assert(machine_match(&m, "a"));
    assert(machine_match(&m, "aa"));
    assert(machine_match(&m, "aaaaaaaaaa"));
    assert(!machine_match(&m, "ab"));

    machine_free(&m);

}
