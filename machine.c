#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


State MATCH_STATE = {MATCH, &MATCH_STATE, NULL};


void push(State **state_list, size_t *list_p, State *s);
bool is_broken(Machine *machine);
bool has_match(Machine *machine);
void step(Machine *machine, int c);


State *state_new(int c, State *next, State *next_2) {
    State *s = malloc(sizeof(State));
    if (!s) {
        perror("state_new");
        exit(EXIT_FAILURE);
    }

    s->c = c;
    s->next = next;
    s->next_2 = next_2;

    return s;
}


void machine_init(Machine *machine, size_t n_states, State *start) {
    machine->start = start;
    machine->current = malloc(sizeof(State *) * n_states);
    machine->cur_p = 0;

    machine->next = malloc(sizeof(State *) * n_states);
    machine->next_p = 0;

    push(machine->current, &(machine->cur_p), start);
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
    free(m->next);
    free(m->current);
}


/*** PRIVATE SECTION ***/


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


/*
 * The machine is considered "broken" when there's no
 * current state. It shall happen after a step where no 
 * match occurred.
 */
bool is_broken(Machine *machine) {
    return machine->cur_p == 0;
}


bool has_match(Machine *machine) {
    for (size_t i = 0; i < machine->cur_p; i++) {
        if (machine->current[i] == &MATCH_STATE) {
            return true;
        }
    }

    return false;
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


