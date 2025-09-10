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


typedef struct Grouping {
    int primaries;
    int alternations;
} Grouping;


typedef struct GroupStack {
    Grouping stack[STACK_SZ];
    int k;
} GroupStack;


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


inline void frag_stack_init(FragStack *s) {
    s->stack_p = 0;
}


inline Fragment frag_stack_pop(FragStack *s) {
    return s->stack[--(s->stack_p)];
}


inline void frag_stack_push(FragStack *s, Fragment f) {
    s->stack[(s->stack_p)++] = f;
}


inline bool frag_stack_empty(FragStack *s) {
    return s->stack_p == 0;
}


inline void group_stack_init(GroupStack *s) {
    s->k = 0;
}


inline Grouping group_stack_pop(GroupStack *s) {
    return s->stack[--(s->k)];
}


inline void group_stack_push(GroupStack *s, Grouping g) {
    s->stack[(s->k)++] = g;
}


inline bool group_stack_empty(GroupStack *s) {
    return s->k == 0;
}


StateList* list(State**);
Fragment fragment(State*, StateList*);
StateList *append(StateList*, StateList*);
void patch(StateList*, State*);

void frag_stack_init(FragStack*);
Fragment frag_stack_pop(FragStack*);
void frag_stack_push(FragStack*, Fragment);
bool frag_stack_empty(FragStack*);

void group_stack_init(GroupStack*);
Grouping group_stack_pop(GroupStack*);
void group_stack_push(GroupStack*, Grouping);
bool group_stack_empty(GroupStack*);
