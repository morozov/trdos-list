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
