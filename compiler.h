#pragma once

#include <stdlib.h>

#include "machine.h"


typedef union StateList {
    State *s;
    union StateList *next;
} StateList;


StateList* list(State **out) {
    return (StateList*) out;
}



typedef struct Fragment {
    State *in;
    StateList *out;
} Fragment;


Fragment fragment(State *in, StateList* out) {
    Fragment f = {in, out};
    return f;
}


void patch(StateList *out, State *in) {
    StateList *next;
    for (; out; out = next) {
        next = out->next;
        out->s = in;
    }
}


StateList *append(StateList *l1, StateList *l2) {
    StateList *cur = l1;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = l2;

    return l1;
}


char *pattern;


Fragment primary() {
    State *s = malloc(sizeof(State));
    if (s == NULL) {
        perror("primary");
        exit(1);
    }

    s->c = *pattern++;
    s->next = NULL;
    s->next_2 = NULL;

    return fragment(s, list(&(s->next)));
}


Fragment concat() {
    Fragment head = primary();
    if (*pattern && *pattern != '|') {
        Fragment tail = concat();
        patch(head.out, tail.in);
        head.out = tail.out;
    }
    return head;
}


Fragment alternation() {
    Fragment left = concat();
    if (*pattern == '|') {
        pattern++;
        Fragment right = alternation();

        State *s = malloc(sizeof(State));
        if (!s) {
            perror("alternation");
            exit(EXIT_FAILURE);
        }

        s->c = BRANCH;
        s->next = left.in;
        s->next_2 = right.in;

        left.in = s;
        left.out = append(left.out, right.out);
    }

    return left;
}



Machine compile_pattern(char *s) {
    pattern = s;
    Fragment f = alternation();
    patch(f.out, &MATCH_STATE);

    Machine m;
    machine_init(&m, 2, f.in);

    return m;
}










