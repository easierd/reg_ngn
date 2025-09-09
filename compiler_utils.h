#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "machine.h"


enum {
    STACK_SZ = 1000,
};


typedef union StateList {
    State *s;
    union StateList *next;
} StateList;


typedef struct Fragment {
    State *in;
    StateList *out;
} Fragment;


typedef struct FragStack {
    Fragment stack[STACK_SZ];
    int stack_p;
} FragStack;



inline StateList* list(State **out) {
    return (StateList*) out;
}


inline Fragment fragment(State *in, StateList* out) {
    Fragment f = {in, out};
    return f;
}


inline StateList *append(StateList *l1, StateList *l2) {
    StateList *cur = l1;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = l2;

    return l1;
}

inline void patch(StateList *out, State *in) {
    StateList *next;
    for (; out; out = next) {
        next = out->next;
        out->s = in;
    }
}


inline void stack_init(FragStack *s) {
    s->stack_p = 0;
}


inline Fragment stack_pop(FragStack *s) {
    return s->stack[--(s->stack_p)];
}


inline void stack_push(FragStack *s, Fragment f) {
    s->stack[(s->stack_p)++] = f;
}


inline bool stack_empty(FragStack *s) {
    return s->stack_p == 0;
}


StateList* list(State**);
Fragment fragment(State*, StateList*);
StateList *append(StateList*, StateList*);
void patch(StateList*, State*);
void stack_init(FragStack*);
Fragment stack_pop(FragStack*);
void stack_push(FragStack*, Fragment);
bool stack_empty(FragStack*);
