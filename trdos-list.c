#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "formats/trd.c"
#include "printer.c"
#include "trdos.h"

void usage(char *program) {
    printf(
        "TR-DOS List is an utility to display the TR-DOS image file "
        "listing.\n\n");
    printf("Usage:\n");
    printf("  %s <file>\n", program);
}

extern int errno;

int list(char *filename) {
    FILE *fp;
    int result;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Unable to open \"%s\": %s\n", filename,
                strerror(errno));

        return 1;
    }

    result = trd_list(fp);

    fclose(fp);

    return result;
}

int main(int argc, char *argv[]) {
    char *filename;

    if (argc < 2) {
        usage(argv[0]);

        return 1;
    }

    return list(argv[1]);
}
