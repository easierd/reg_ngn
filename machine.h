#pragma once
#include <stdbool.h>
#include <stdlib.h>

enum {
    MATCH = -1,
    BRANCH = -2,
};

typedef struct State {
    int c;
    struct State *next;
    struct State *next_2;
} State;


typedef struct Machine {
    State **current;
    size_t cur_p;
    State **next;
    size_t next_p;
} Machine;


/*
 * Sentinel to represent the final state 
 */
State MATCH_STATE = {MATCH, NULL, NULL};


/*
 * This function must be used for every insert operation in the 
 * two vectors of the machine. Indeed, it guarantees that the
 * machine'll never be in a BRANCH state.
 */
void push(State **state_list, size_t *list_p, State *s) {
    if (s->c == BRANCH) {
        push(state_list, list_p, s->next);
        push(state_list, list_p, s->next_2);
    } else {
        state_list[(*list_p)++] = s;
    }
}


void machine_init(Machine *machine, size_t n_states, State *start) {
    machine->current = malloc(sizeof(State *) * n_states);
    machine->cur_p = 0;

    machine->next = malloc(sizeof(State *) * n_states);
    machine->next_p = 0;

    push(machine->current, &(machine->cur_p), start);
}


/*
 * The machine is considered "broken" when there's no
 * current state. It shall happen after a step where no 
 * match occurred.
 */
bool is_broken(Machine *machine) {
    return machine->cur_p == 0;
}


void step(Machine *machine, int c) {
    for (size_t i = 0; i < machine->cur_p; i++) {
        State *state = machine->current[i];

        /*
         * The machine is guaranteed not to be in a 
         * SPLIT state so we don't need to check.
         */
        if (c == state->c) {
            push(machine->next, &(machine->next_p), state->next);
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



bool has_match(Machine *machine) {
    for (size_t i = 0; i < machine->cur_p; i++) {
        if (machine->current[i] == &MATCH_STATE) {
            return true;
        }
    }

    return false;
}


bool machine_match(Machine *machine, char *string) {
    for (;*string; string++) {
        step(machine, *string);
        if (is_broken(machine)) {
            return false;
        }
    }
    if (has_match(machine)) {
            return true;
    }
    return false;
}



void machine_free(Machine *m) {
    free(m->current);
    free(m->next);
}
