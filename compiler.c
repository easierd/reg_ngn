#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "compiler_utils.h"
#include "machine.h"


static State *compile(char *s, long *states) {
    // match state is always present
    *states = 1;

    if (!*s) {
        return &MATCH_STATE;
    }

    FragStack stack;
    stack_init(&stack);

    int primaries = 0;
    Fragment f1;
    Fragment f2;

    for (; *s; s++) {
        switch(*s) {
            case '(':
            case ')':
            case '|':
            case '*':
            case '+':
            case '?':
                break;
            default:
                if (primaries > 1) {
                    primaries--;
                    f1 = stack_pop(&stack);
                    f2 = stack_pop(&stack);
                    patch(f2.out, f1.in);
                    f2.out = f1.out;
                    stack_push(&stack, f2);
                }
                primaries++;

                State *state = malloc(sizeof(State));
                state->c = *s;
                state->next = NULL;
                state->next_2 = NULL;

                states++;

                Fragment f = fragment(state, list(&(state->next)));
                stack_push(&stack, f);

                break;
        }
    }

    while(--primaries > 0) {
        f1 = stack_pop(&stack);
        f2 = stack_pop(&stack);
        patch(f2.out, f1.in);
        f2.out = f1.out;
        stack_push(&stack, f2);
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
