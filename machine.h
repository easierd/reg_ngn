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


enum {
    SV_DEF_CAPACITY=8,
};

typedef struct StateVector {
    State **state;
    size_t sz;
    size_t capacity;
} StateVector;

StateVector *sv_new();
void sv_push(StateVector*, State*);
void sv_free(StateVector*);


typedef struct Machine {
    State *start;
    State **current;
    size_t cur_p;
    State **next;
    size_t next_p;
    int iter;
    StateVector *sv;
} Machine;


/*
 * Sentinel to represent the final state 
 */
extern State MATCH_STATE;


State *state_new(int, State*, State*);

void machine_init(Machine *machine, StateVector *sv, State *start);
bool machine_match(Machine *machine, char *string);
void machine_free(Machine *m);
