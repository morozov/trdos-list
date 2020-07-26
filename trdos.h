#include <stdint.h>

#define SECTOR_SIZE 256
#define INFO_SECTOR 8

#define OFFSET_DISK_FORMAT 227
#define OFFSET_TOTAL_FILES 228
#define OFFSET_FREE_SECTORS 229
#define OFFSET_TRDOS_IDENTIFIER 231
#define OFFSET_DELETED_FILES 244
#define OFFSET_LABEL 245

#define FORMAT_80_TRACKS_DOUBLE_SIDE 0x16
#define FORMAT_40_TRACKS_DOUBLE_SIDE 0x17
#define FORMAT_80_TRACKS_SINGLE_SIDE 0x18
#define FORMAT_40_TRACKS_SINGLE_SIDE 0x19

#define TRDOS_IDENTIFIER 0x10

typedef struct {
    char format;
    unsigned char label[8];
    uint8_t num_total_files;
    uint8_t num_deleted_files;
    uint16_t num_free_sectors;
} disk_info;

typedef struct {
    unsigned char filename[8];
    unsigned char extension;
    uint16_t start_address;
    uint16_t length_bytes;
    uint8_t length_sectors;
} file_info;
