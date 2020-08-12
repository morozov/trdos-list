#ifndef _PRINTER_C
#define _PRINTER_C 1

#include <stdio.h>
#include <string.h>

#include "trdos.h"
#include "trdos.c"

void print_padding(int number) {
    if (number < 100) {
        printf(" ");
    }

    if (number < 10) {
        printf(" ");
    }
}

void print_disk_format(int format) {
    switch (format) {
        case FORMAT_80_TRACKS_DOUBLE_SIDE:
            printf("80 Track D. Side");
            break;
        case FORMAT_40_TRACKS_DOUBLE_SIDE:
            printf("40 Track D. Side");
            break;
        case FORMAT_80_TRACKS_SINGLE_SIDE:
            printf("80 Track S. Side");
            break;
        case FORMAT_40_TRACKS_SINGLE_SIDE:
            printf("40 Track S. Side");
            break;
    }
}

void print_disk_info(disk_info *info) {
    printf("Title:  %.8s Disk Drive: A\n", info->label);

    printf("%d File(s)     ", info->num_total_files);
    print_padding(info->num_total_files);
    print_disk_format(info->format);
    printf("\n");

    printf("%d Del. File(s)", info->num_deleted_files);
    print_padding(info->num_deleted_files);
    printf("Free Sector %d\n", info->num_free_sectors);

    printf("\n");
    printf("  File Name    Start Length Line\n");
}

/**
 * Reads the autostart line of a BASIC file from the file pointer
 * into the file info struct
 *
 * @param fp     The image file pointer
 * @param info   The target file info struct
 * @param offset The offset to subtract from the file end position
 *
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on failure
 */
int print_read_autostart(FILE *fp, file_info *info, int offset) {
    const char *marker = "\x80\xAA";
    unsigned char buffer[4];

    fseek(fp, trdos_file_end_offset(info) - offset, SEEK_SET);

    if (fread(buffer, 1, 4, fp) != 4) {
        perror("Unable to read autostart data");

        return EXIT_FAILURE;
    }

    if (strncmp(buffer, marker, 2) == 0) {
        info->autostart_line = buffer[2] | (buffer[3] << 8);
    }

    return EXIT_SUCCESS;
}

int print_file_info(file_info *info, FILE *fp, int offset) {
    int pos;
    int result;

    switch (info->filename[0]) {
        case 0x00:
        case 0x01:
            return EXIT_SUCCESS;
    }

    if (info->extension == 'B') {
        pos = ftell(fp);

        result = print_read_autostart(fp, info, offset);
        fseek(fp, pos, SEEK_SET);

        if (result != EXIT_SUCCESS) {
            return result;
        }
    }

    printf("%.8s<%c>%3d  %05d %05d", info->filename, info->extension,
           info->length_sectors, info->start_address, info->length_bytes);

    if (info->autostart_line > 0) {
        printf(" %d", info->autostart_line);
    }

    printf("\n");

    return EXIT_SUCCESS;
}
#endif