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


Fragment alternation();


StateList *append(StateList *l1, StateList *l2) {
    StateList *cur = l1;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = l2;

    return l1;
}


char *pattern;
long states = 0;


Fragment primary() {
    if (*pattern == '(') {
        pattern ++;
        Fragment grouping = alternation();
        pattern ++;
        return grouping;
    }
    State *s = malloc(sizeof(State));
    states++;
    if (s == NULL) {
        perror("primary");
        exit(1);
    }

    s->c = *pattern;
    s->next = NULL;
    s->next_2 = NULL;

    if (*pattern) {
        pattern++;
    }

    return fragment(s, list(&(s->next)));
}

Fragment kleene() {
    Fragment left = primary();
    if (*pattern == '*') {
        pattern++;
        State *s = malloc(sizeof(State));
        states++;
        if (!s) {
            perror("kleene");
            exit(EXIT_FAILURE);
        }

        s->c = BRANCH;
        s->next = left.in;
        s->next_2 = NULL;

        patch(left.out, s);
        left.in = s;
        left.out = list(&(s->next_2));
    }

    return left;
}


Fragment optionality() {
    Fragment left = kleene();
    if (*pattern == '?') {
        pattern++;
        State *s = malloc(sizeof(State));
        states++;
        if (!s) {
            perror("optionality");
            exit(EXIT_FAILURE);
        }
        s->c = BRANCH;
        s->next = left.in;
        s->next_2 = NULL;
        
        left.in = s;
        left.out = append(left.out, list(&(s->next_2)));
    }

    return left;
}


Fragment concat() {
    Fragment head = optionality();

    if (*pattern && *pattern != '|' && *pattern != ')') {
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
    machine_init(&m, states, f.in);

    return m;
}










