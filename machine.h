#pragma once
#include <stdbool.h>
#include <stdlib.h>

typedef struct State {
    int c;
    struct State *next;
} State;


typedef struct Machine {
    State **current;
    size_t cur_p;
    State **next;
    size_t next_p;
} Machine;


State MATCH_STATE = {-1, NULL};


void machine_init(Machine *machine, size_t n_states, State *start) {
    machine->current = malloc(sizeof(State *) * n_states);
    machine->cur_p = 0;

    machine->next = malloc(sizeof(State *) * n_states);
    machine->next_p = 0;

    machine->current[machine->cur_p++] = start;
}


/*
 * The machine is considered "broken" when there's no
 * current state. It shall happen after a step where no 
 * match occurred.
 */
bool machine_broken(Machine *machine) {
    return machine->cur_p == 0;
}


void machine_step(Machine *machine, int c) {
    for (size_t i = 0; i < machine->cur_p; i++) {
        State *state = machine->current[i];
        if (c == state->c) {
            machine->next[machine->next_p++] = state->next;
        }
    }

    /* 
     * Swap current and next in order
     * to keep the invariant.
     */
    State **temp = machine->current;
    machine->current = machine->next;
    machine->next = temp;

    machine->cur_p = machine->next_p;
    machine->next_p = 0;
}



bool machine_has_match(Machine *machine) {
    for (size_t i = 0; i < machine->cur_p; i++) {
        if (machine->current[i] == &MATCH_STATE) {
            return true;
        }
    }

    return false;
}


void machine_free(Machine *m) {
    free(m->current);
    free(m->next);
}
