#pragma once

typedef struct State {
    int c;
    struct State *next;
} State;


typedef struct Machine {
    State *state;
} Machine;


State MATCH_STATE = {-1, NULL};

