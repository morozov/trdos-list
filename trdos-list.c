#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trdos.h"

void usage(char *program) {
    printf(
        "TR-DOS List is an utility to display the TR-DOS image file "
        "listing.\n\n");
    printf("Usage:\n");
    printf("  %s <file>\n", program);
}

extern int errno;

void trd_unpack_disk_info(unsigned char *buffer, disk_info *info) {
    info->format = buffer[OFFSET_DISK_FORMAT];
    info->num_total_files = buffer[OFFSET_TOTAL_FILES];
    info->num_free_sectors =
        buffer[OFFSET_FREE_SECTORS] | (buffer[OFFSET_FREE_SECTORS + 1] << 8);
    info->num_deleted_files = buffer[OFFSET_DELETED_FILES];

    strncpy(info->label, buffer + OFFSET_LABEL, 8);
}

void trd_unpack_file_info(unsigned char *buffer, file_info *info) {
    strncpy(info->filename, buffer, 8);

    info->extension = buffer[8];
    info->start_address = buffer[9] | (buffer[10] << 8);
    info->length_bytes = buffer[11] | (buffer[12] << 8);
    info->length_sectors = buffer[13];
}

int trd_read_disk_info(FILE *fp, disk_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(SECTOR_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Unable to allocate buffer\n");

        return 1;
    }

    if (fread(buffer, 1, SECTOR_SIZE, fp) != SECTOR_SIZE) {
        fprintf(stderr, "Unexpected end of file\n");
        free(buffer);

        return 1;
    }

    if (buffer[OFFSET_TRDOS_IDENTIFIER] != TRDOS_IDENTIFIER) {
        fprintf(stderr, "Not a TR-DOS image\n");
        free(buffer);

        return 1;
    }

    trd_unpack_disk_info(buffer, info);
    free(buffer);

    return 0;
}

int trd_read_file_info(FILE *fp, file_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(16);
    if (buffer == NULL) {
        fprintf(stderr, "Unable to allocate buffer\n");

        return 1;
    }

    if (fread(buffer, 1, 16, fp) != 16) {
        fprintf(stderr, "Unexpected end of file\n");
        free(buffer);

        return 1;
    }

    trd_unpack_file_info(buffer, info);
    free(buffer);

    return 0;
}

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

void print_file_info(file_info *info) {
    printf("%.8s<%c>%3d  %05d %05d\n", info->filename, info->extension,
           info->length_sectors, info->start_address, info->length_bytes);
}

int trd_list(FILE *fp) {
    int result;
    disk_info disk_info;
    file_info file_info;

    fseek(fp, INFO_SECTOR * SECTOR_SIZE, SEEK_SET);

    result = trd_read_disk_info(fp, &disk_info);
    if (result != 0) {
        return result;
    }

    print_disk_info(&disk_info);

    fseek(fp, 0, SEEK_SET);

    for (unsigned char i = 0; i < disk_info.num_total_files; i++) {
        result = trd_read_file_info(fp, &file_info);
        if (result != 0) {
            return result;
        }

        print_file_info(&file_info);
    }

    return 0;
}

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
