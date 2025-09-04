#pragma once


#include <stdlib.h>

#include "machine.h"


static long states = 0;
char *pattern;


// literal := {ASCII}
State *literal() {
    states++;
    if (!*pattern) {
        return &MATCH_STATE;
    }

    State *state = malloc(sizeof(State));
    state->c = *(pattern++);
    state->next = &MATCH_STATE;
    state->next_2 = NULL;

    return state;
}


// concat := literal (concat)*
State *concat() {
    State *head = literal();

    if (*pattern && *pattern != '|') {
        State *next = concat();
        head->next = next;
    }

    return head;
}


// alternation := concat `|` alternation
// (left associative)
State *alternation() {
    State *head = concat();

    if (*pattern == '|') {
        pattern++;
        State *alternative = alternation();

        State *branch = malloc(sizeof(State));
        branch->c = BRANCH;
        branch->next = head;
        branch->next_2 = alternative;
        
        head = branch;
    }

    return head;
}


Machine compile_pattern(char *s) {
    pattern = s;
    State *start = alternation();
    Machine m;
    machine_init(&m, states, start);

    return m;
}
