#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "formats/scl.c"
#include "formats/trd.c"
#include "printer.c"
#include "trdos.h"

void usage(char *program) {
    printf(
        "TR-DOS List is an utility to display the TR-DOS image file "
        "listing.\n\n");
    printf("Usage:\n");
    printf("  %s <file.trd>\n", program);
    printf("  %s <file.scl>\n", program);
}

extern int errno;

int has_ext(char *filename, const char *ext) {
    const char *pos = strrchr(filename, '.');

    if (pos == NULL) {
        return 1;
    }

    return strncasecmp(pos + 1, ext, strlen(ext));
}

int list(char *filename) {
    FILE *fp;
    int result;
    int (*fun)(FILE *);

    if (has_ext(filename, "trd") == 0) {
        fun = &trd_list;
    } else if (has_ext(filename, "scl") == 0) {
        fun = &scl_list;
    } else {
        printf("Unsupported file type\n");

        return EXIT_FAILURE;
    }

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Unable to open \"%s\": %s\n", filename,
                strerror(errno));

        return EXIT_FAILURE;
    }

    result = fun(fp);

    fclose(fp);

    return result;
}

int main(int argc, char *argv[]) {
    char *filename;

    if (argc < 2) {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    return list(argv[1]);
}
