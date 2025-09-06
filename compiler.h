#pragma once

#include <stdlib.h>

#include "machine.h"


typedef struct StateList {
    State *s;
    struct StateList *next;
} StateList;


typedef struct Fragment {
    State *in;
    StateList *out;
} Fragment;

Machine *compile_pattern(char *s) {
    return NULL;
}
