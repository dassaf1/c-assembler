#include <stdlib.h>
#include "assembler.h"
#define ASSEMBLY_FILE_EXTENSION ".as"

int main(int argc, char * argv[]) {
    FILE * fd;
    int i;

    /* verify command line arguments */
    if (argc == 1) {
        fprintf(stderr, "ERROR: You must enter at least 1 assembly (%s) input file name as a command line argument\n", ASSEMBLY_FILE_EXTENSION);
        exit(2);
    }

    /* verify files exist, files extension, file length and read permissions */
    for (i = 1; i < argc; i++) {
        char * filename = argv[i];

        /* check filename length and extension */
        if (strlen(filename) < 4) {
            fprintf(stderr, "ERROR: file %s is too short.\n", filename);
            exit(2);
        }
        else { /* check file extension */
            char * file_extension = &filename[strlen(filename)-3];
            if (strcmp(file_extension, ASSEMBLY_FILE_EXTENSION) != 0) {
                fprintf(stderr, "ERROR: file %s has not the right extension (%s)\n", filename, ASSEMBLY_FILE_EXTENSION);
                exit(2);
            }
        }
        /* check files exist and permissions */
        if (!(fd = fopen(filename, "r"))) {
            fprintf(stderr, "ERROR: There is no such file %s or you don't have read permissions on it\n", filename);
            exit(2);
        }
    }

    for (i = 1; i < argc; i++) {
        char * filename = argv[i];
        fd = fopen(argv[i], "r");
        printf("INFO: Running assembler on file %s\n", filename);
        if (execute_first_pass(fd)) {
            printf("INFO: First pass run completed successfully on file %s\n", filename);
            printf("INFO: Running second pass on file %s\n", filename);
            execute_second_pass(filename);
            printf("INFO: Finished running assembler on file %s\n", filename);
            printf("-----------------------------------------------------------------\n");
        }
        else {
            fprintf(stderr, "ERROR: Errors found in first pass. Second pass will not be run.\n");
        }
        fclose(fd);
    }

    return 0;
}