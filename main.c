#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>


// Your code here
int main(int argc, char *argv[]) {
    /*if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }*/

    FILE *file = fopen("processed_trace", "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // Start ------------------

    // array of blocks
    size_t start_index = 0;
    for (int i = 0; i < 10; i++) {
        block_t* block = parse_block(file, start_index, 5);
        start_index = block->end_index + 1;
        print_block(file, block);
    }
    

    // End --------------------
    fclose(file);
    return EXIT_SUCCESS;
}