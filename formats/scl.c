#include <stdlib.h>
#include <string.h>

#include "../printer.c"
#include "../trdos.h"

#define SCL_HEADER "SINCLAIR"
#define SCL_LABEL "        "

void scl_unpack_file_info(const unsigned char *buffer, file_info *info) {
    strncpy(info->filename, buffer, 8);

    info->extension = buffer[8];
    info->start_address = buffer[9] | (buffer[10] << 8);
    info->length_bytes = buffer[11] | (buffer[12] << 8);
    info->length_sectors = buffer[13];
}

int scl_read_disk_info(FILE *fp, disk_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(9);

    if (buffer == NULL) {
        perror("Unable to read disk info");

        return 1;
    }

    if (fread(buffer, 1, 9, fp) != 9) {
        perror("Unable to read header");

        return 1;
    }

    if (strncmp(buffer, SCL_HEADER, 8) != 0) {
        fprintf(stderr, "Not an SCL image\n");
        free(buffer);

        return 1;
    }

    info->format = FORMAT_80_TRACKS_DOUBLE_SIDE;
    info->num_total_files = buffer[8];
    info->num_deleted_files = 0;
    info->num_free_sectors = 2544;

    strncpy(info->label, SCL_LABEL, 8);

    free(buffer);

    return 0;
}

int scl_read_file_info(FILE *fp, file_info *info) {
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(14);
    if (buffer == NULL) {
        perror("Unable to read file info");

        return 1;
    }

    if (fread(buffer, 1, 14, fp) != 14) {
        perror("Unable to read file info");
        free(buffer);

        return 1;
    }

    scl_unpack_file_info(buffer, info);
    free(buffer);

    return 0;
}

int scl_list(FILE *fp) {
    int result;
    int header_size;
    int curr_sector = SIZE_TRACK_SECTORS;
    disk_info disk_info;

    result = scl_read_disk_info(fp, &disk_info);
    if (result != 0) {
        return result;
    }

    file_info *file_infos =
        malloc(sizeof(file_info) * disk_info.num_total_files);

    for (unsigned char i = 0; i < disk_info.num_total_files; i++) {
        result = scl_read_file_info(fp, &file_infos[i]);
        if (result != 0) {
            free(file_infos);

            return result;
        }

        file_infos[i].start_track = curr_sector / SIZE_TRACK_SECTORS;
        file_infos[i].start_sector = curr_sector % SIZE_TRACK_SECTORS;
        curr_sector += file_infos[i].length_sectors;

        disk_info.num_free_sectors -= file_infos[i].length_sectors;
    }

    // this is where the SCL header ends
    header_size = ftell(fp);

    print_disk_info(&disk_info);

    for (unsigned char i = 0; i < disk_info.num_total_files; i++) {
        result = print_file_info(
            &file_infos[i], fp,
            SIZE_TRACK_SECTORS * SIZE_SECTOR_BYTES - header_size);

        if (!result) {
            free(file_infos);

            return result;
        }
    }

    free(file_infos);

    return 1;
}
