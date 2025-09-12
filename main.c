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

    machine_free(&m);

    return EXIT_SUCCESS;
}
