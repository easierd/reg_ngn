#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "compiler_utils.h"
#include "machine.h"


void concat_last_two(FragStack *s) {
    Fragment f1 = frag_stack_pop(s);
    Fragment f2 = frag_stack_pop(s);
    patch(f2.out, f1.in);
    f2.out = f1.out;
    frag_stack_push(s, f2);
}


void alternate_last_two(FragStack *s, StateVector *sv) {
    Fragment f1 = frag_stack_pop(s);
    Fragment f2 = frag_stack_pop(s);
    State *branch = state_new(BRANCH, f1.in, f2.in);
    sv_push(sv, branch);
    f2.out = append(f1.out, f2.out);
    f2.in = branch;
    frag_stack_push(s, f2);
}


void optional_last(FragStack *s, StateVector *sv) {
    Fragment f = frag_stack_pop(s);
    State *branch = state_new(BRANCH, f.in, NULL);
    sv_push(sv, branch);
    f.out = append(f.out, list(&(branch->next_2)));
    f.in = branch;
    frag_stack_push(s, f);
}


void kleene_last(FragStack *s, StateVector *sv) {
    Fragment f = frag_stack_pop(s);
    State *branch = state_new(BRANCH, f.in, NULL);
    sv_push(sv, branch);
    patch(f.out, branch);
    f.out = list(&(branch->next_2));
    f.in = branch;
    frag_stack_push(s, f);
}


void one_or_more_last(FragStack *s, StateVector *sv) {
    Fragment f = frag_stack_pop(s);
    State *branch = state_new(BRANCH, f.in, NULL);
    sv_push(sv, branch);
    patch(f.out, branch);
    f.out = list(&branch->next_2);
    frag_stack_push(s,f);
}


static State *compile(char *s, StateVector *sv) {
    // match state is always present
    if (!*s) {
        return &MATCH_STATE;
    }

    FragStack stack;
    frag_stack_init(&stack);

    GroupStack gstack;
    group_stack_init(&gstack);
    Grouping g = {0, 0};

    for (; *s; s++) {
        switch(*s) {
            case '(':
                if (g.primaries > 1) {
                    g.primaries--;
                    concat_last_two(&stack);
                }
                group_stack_push(&gstack, g);
                g.primaries = 0;
                g.alternations = 0;
                break;

            case ')':
                if (group_stack_empty(&gstack) || g.primaries == 0) {
                    return NULL;
                }
                while(--g.primaries > 0) {
                    concat_last_two(&stack);
                }
                while (g.alternations-- > 0) {
                    alternate_last_two(&stack, sv);
                }
                g = group_stack_pop(&gstack);
                g.primaries++;
                break;

            case '|':
                while (--g.primaries > 0) {
                    concat_last_two(&stack);
                }
                g.alternations++;
                break;

            case '?':
                if (g.primaries == 0) {
                    return NULL;
                } 
                optional_last(&stack, sv); 
                break;

            case '*':
                if (g.primaries == 0) {
                    return NULL;
                }
                kleene_last(&stack, sv);
                break;

            case '+':
                if (g.primaries == 0) {
                    return NULL;
                }
                one_or_more_last(&stack, sv);
                break;

            default:
                if (g.primaries > 1) {
                    g.primaries--;
                    concat_last_two(&stack);
                }
                g.primaries++;

                State *state = (*s == '.') ? state_new(ANY, NULL, NULL) : 
                                           state_new(*s, NULL, NULL);
                if (!state) {
                    perror("compile");
                    exit(EXIT_FAILURE);
                }
                sv_push(sv, state);

                Fragment f = fragment(state, list(&(state->next)));
                frag_stack_push(&stack, f);
                break;
        }
    }

    while(--g.primaries > 0) {
        concat_last_two(&stack);
    }

    while (g.alternations-- > 0) {
        alternate_last_two(&stack, sv);
    }

    Fragment last = frag_stack_pop(&stack);
    if (!frag_stack_empty(&stack)) {
        return NULL;
    }
    patch(last.out, &MATCH_STATE);

    return last.in;
}


Machine compile_pattern(char *s) {
    StateVector *sv = sv_new();
    State *start = compile(s, sv);
    if (start == NULL) {
        fprintf(stderr, "Bad regexp\n");
        exit(EXIT_FAILURE);
    }
    Machine m;
    machine_init(&m, sv, start);

    return m;
}
