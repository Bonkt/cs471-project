#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace-selection/datatype.h"
#include "trace-selection/parser.h"


// Your code here
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // Start ------------------

    inst_t inst = {0};
    block_t *block = parse_block(file, 0, 10);
    print_block(file, block);

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}