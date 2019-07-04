#include <stdio.h>
#include <liboddl/liboddl.h>

void print_help(char* cmd) {
    printf("Usage: %s <filename>\n", cmd);
}

int main(int argc, char** argv) {
    FILE* file;
    struct ODDLDoc oddlDoc;
    int success;

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (!(file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: can't open file %s\n", argv[1]);
        return 1;
    }

    success = oddl_parse(&oddlDoc, file);
    if (!success) {
        fprintf(stderr, "Error in parsing module\n");
        return 1;
    }

    oddl_write(&oddlDoc, stdout);
    oddl_free(&oddlDoc);
    fclose(file);
    return 0;
}
