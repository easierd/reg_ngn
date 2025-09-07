#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "machine.h"
#include "compiler.h"


int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: match [pattern] [string]\n");
        return EXIT_FAILURE;
    }

    Machine m = compile_pattern(*++argv);

    if (machine_match(&m, *++argv)){
        printf("%s\n", *argv);
    }

    /* TL;DR: the program has memory leaks, but 
     * it's supposed to run and exit, so it's not
     * as much of a loss.
     *
     * The memory management is a bit tricky because 
     * the machine can contain loops (I shall study 
     * some technique for directed graphs)
     */
    machine_free(&m);

    return EXIT_SUCCESS;
}
