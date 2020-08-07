/**
 * Returns the offset of the given file end relatively to the start of the disk
 */
int trdos_file_end_offset(file_info *info) {
    return ((SIZE_TRACK_SECTORS * info->start_track + info->start_sector) *
            SIZE_SECTOR_BYTES) +

           // the start_address here represents the basic file length
           // for some reason
           info->start_address;
}
