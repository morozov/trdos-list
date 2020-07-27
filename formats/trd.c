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