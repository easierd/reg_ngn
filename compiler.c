#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "compiler_utils.h"
#include "machine.h"


void concat_last_two(FragStack *s) {
    Fragment f1 = stack_pop(s);
    Fragment f2 = stack_pop(s);
    patch(f2.out, f1.in);
    f2.out = f1.out;
    stack_push(s, f2);
}


void alternate_last_two(FragStack *s) {
    Fragment f1 = stack_pop(s);
    Fragment f2 = stack_pop(s);

    State *branch = state_new(BRANCH, f1.in, f2.in);
    f2.out = append(f1.out, f2.out);
    f2.in = branch;
    stack_push(s, f2);
}


void optional_last(FragStack *s) {
    Fragment f = stack_pop(s);
    State *branch = state_new(BRANCH, f.in, NULL);
    f.out = append(f.out, list(&(branch->next_2)));
    f.in = branch;
    stack_push(s, f);
}


static State *compile(char *s, long *states) {
    // match state is always present
    *states = 1;

    if (!*s) {
        return &MATCH_STATE;
    }

    FragStack stack;
    stack_init(&stack);

    int primaries = 0;
    int alternations = 0;

    for (; *s; s++) {
        switch(*s) {
            case '(':
            case ')':
            case '*':
            case'+':
                break;
            default:
                if (primaries > 1) {
                    primaries--;
                    concat_last_two(&stack);
                }
                primaries++;

                State *state = state_new(*s, NULL, NULL);
                if (!state) {
                    perror("compile");
                    exit(EXIT_FAILURE);
                }
                (*states)++;

                Fragment f = fragment(state, list(&(state->next)));
                stack_push(&stack, f);

                break;

            case '|':
                while (--primaries > 0) {
                    concat_last_two(&stack);
                }
                alternations++;

                break;
            case '?':
                if (primaries == 0) {
                    return NULL;
                } 
                optional_last(&stack); 
                break;
        }
    }

    while(--primaries > 0) {
        concat_last_two(&stack);
    }

    while (alternations-- > 0) {
        alternate_last_two(&stack);
    }

    Fragment last = stack_pop(&stack);
    if (!stack_empty(&stack)) {
        return NULL;
    }
    patch(last.out, &MATCH_STATE);

    return last.in;
}


Machine compile_pattern(char *s) {
    long states = 0;
    State *start = compile(s, &states);
    if (start == NULL) {
        fprintf(stderr, "Bad regexp\n");
        exit(EXIT_FAILURE);
    }
    Machine m;
    machine_init(&m, states, start);

    return m;
}
