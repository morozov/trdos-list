#include <stdlib.h>
#include <string.h>

#include "../printer.c"
#include "../trdos.h"

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
    info->start_sector = buffer[14];
    info->start_track = buffer[15];
}

int trd_read_disk_info(FILE *fp, disk_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(SIZE_SECTOR_BYTES);
    if (buffer == NULL) {
        perror("Unable to read disk info");

        return 0;
    }

    if (fread(buffer, 1, SIZE_SECTOR_BYTES, fp) != SIZE_SECTOR_BYTES) {
        perror("Unable to read disk info");
        free(buffer);

        return 0;
    }

    if (buffer[OFFSET_TRDOS_IDENTIFIER] != TRDOS_IDENTIFIER) {
        fprintf(stderr, "Not a TR-DOS image\n");
        free(buffer);

        return 0;
    }

    trd_unpack_disk_info(buffer, info);
    free(buffer);

    return 1;
}

int trd_read_file_info(FILE *fp, file_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(16);
    if (buffer == NULL) {
        perror("Unable to read file info");

        return 0;
    }

    if (fread(buffer, 1, 16, fp) != 16) {
        perror("Unable to read file info");
        free(buffer);

        return 0;
    }

    trd_unpack_file_info(buffer, info);
    free(buffer);

    return 1;
}

int trd_list(FILE *fp) {
    int result;
    disk_info disk_info;
    file_info file_info;

    fseek(fp, INFO_SECTOR * SIZE_SECTOR_BYTES, SEEK_SET);

    result = trd_read_disk_info(fp, &disk_info);
    if (!result) {
        return result;
    }

    print_disk_info(&disk_info);

    fseek(fp, 0, SEEK_SET);

    for (unsigned char i = 0; i < disk_info.num_total_files; i++) {
        memset(&file_info, 0, sizeof(file_info));
        result = trd_read_file_info(fp, &file_info);
        if (!result) {
            return result;
        }

        result = print_file_info(&file_info, fp, 0);
        if (!result) {
            return result;
        }
    }

    return 1;
}
