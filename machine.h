#pragma once
#include <stdbool.h>
#include <stdlib.h>

enum {
    MATCH = -1,
    BRANCH = -2,
    ANY = -3,
};

typedef struct State {
    int c;
    struct State *next;
    struct State *next_2;
    int last_iter;
} State;


typedef struct Machine {
    State *start;
    State **current;
    size_t cur_p;
    State **next;
    size_t next_p;
    int iter;
} Machine;


/*
 * Sentinel to represent the final state 
 */
extern State MATCH_STATE;


State *state_new(int, State*, State*);

void machine_init(Machine *machine, size_t n_states, State *start);
bool machine_match(Machine *machine, char *string);
void machine_free(Machine *m);
