#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"


State MATCH_STATE = {MATCH, &MATCH_STATE, NULL, 0};


void push(State **state_list, size_t *list_p, State *s, int iter);
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
    s->last_iter = 0;

    return s;
}


StateVector *sv_new() {
    StateVector *sv = malloc(sizeof(StateVector));
    if (sv == NULL) {
        perror("sv_new");
        exit(EXIT_FAILURE);
    }
    sv->state = malloc(sizeof(State*) * SV_DEF_CAPACITY);    
    if (sv->state == NULL) {
        free(sv);
        perror("sv_new");
        exit(EXIT_FAILURE);
    }
    sv->capacity = SV_DEF_CAPACITY;
    sv->sz = 0;

    return sv;
}


void sv_push(StateVector *sv, State *s) {
    if (sv->capacity == sv->sz) {
        sv->state = realloc(sv->state, sv->capacity * 2 * sizeof(State*));
        sv->capacity *= 2;
    }

    sv->state[(sv->sz)++] = s;
}


size_t sv_len(StateVector *sv) {
    return sv->sz;
}


void sv_free(StateVector *sv) {
    while((sv->sz)-- > 0) {
        free(sv->state[sv->sz]);
    }
    free(sv->state);
    free(sv);
}


void machine_init(Machine *machine, StateVector *sv, State *start) {
    machine->start = start;
    machine->current = malloc(sizeof(State *) * (sv_len(sv) + 1));
    machine->cur_p = 0;

    machine->next = malloc(sizeof(State *) * (sv_len(sv) + 1));
    machine->next_p = 0;

    machine->sv = sv;

    machine->iter = 1;
    push(machine->current, &(machine->cur_p), start, machine->iter);
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
    sv_free(m->sv);
}


/*** PRIVATE SECTION ***/


/*
 * This function must be used for every insert operation in the 
 * two vectors of the machine. Indeed, it guarantees that the
 * machine'll never be in a BRANCH state.
 */
void push(State **state_list, size_t *list_p, State *s, int iter) {
    if (s->last_iter == iter) {
        return;
    }
    s->last_iter = iter;
    if (s->c == BRANCH) {
        push(state_list, list_p, s->next, iter);
        push(state_list, list_p, s->next_2, iter);
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
    machine->iter++;
    for (size_t i = 0; i < machine->cur_p; i++) {
        State *state = machine->current[i];

        /*
         * The machine is guaranteed not to be in a 
         * SPLIT state so we don't need to check.
         */
        if (c == state->c || state->c == ANY) {
            push(machine->next, &(machine->next_p), state->next, machine->iter);
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


