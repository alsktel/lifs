// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_FILE_H__
#define __LIFS_FILE_H__

#include <lifs_commons.h>

#define _LIFS_FILE_HEADER_SIZE_ 512
#define _LIFS_FILE_INFO_SIZE_ 48
#define _LIFS_FILE_RESERVED_SIZE_ 512 - 48
#define _LIFS_NODE_DATA_COUNT_ 126
#define _LIFS_FILE_EOF_ 0x1C
#define _LIFS_FILE_ATTRIBUTES_DEFAULT_ 0b00000000000000000000000011000000
#define _LIFS_DIRECTORY_ATTRIBUTES_DEFAULT_ _LIFS_FILE_ATTRIBUTES_DEFAULT_ | 1
#define _LIFS_FILE_NEXT_FIELD_OFFSET_ _LIFS_NAMES_LEN_ + 0x10 + 0x08 + 0x04
#define _LIFS_FILE_SECTOR_FIELD_OFFSET_ _LIFS_NAMES_LEN_ + 0x10

// This struct describes file header
typedef struct 
{
    uint8_t name[_LIFS_NAMES_LEN_]; // Name in UTF-8
    uint32_t link; // Real entry for links (see docs)
    uint32_t size; // Size in sectors
    uint32_t flags; // File attributes (see docs)
    uint32_t content; // Local id of sector with content (see docs)
    uint32_t sector; // Local id of sector with this header
    uint32_t parent; // Local id of sector with parent directory header
    uint32_t previous; // Local id of sector with previous file header
    uint32_t next; // Local id of sector with next file header
    uint32_t additional; // Additional data (reserved, see docs)
    uint32_t created; // File creation time in FLIX timestamp
    uint32_t edited; // File last modification time in FLIX timestamp
    uint32_t accessed; // File last access time in FLIX timestamp
    uint8_t reserved[_LIFS_FILE_RESERVED_SIZE_]; // Reserved
} lifs_file_t;

// Creates file header with specific parameters
// Returns pointer to header
lifs_file_t* create_file(const char* name, uint32_t flags, uint32_t size, 
    uint32_t sector, uint32_t content, uint32_t parent, uint32_t previous);

// Converts file to LIFS uint on partition with header 
// on sector 'partition' on disk 'disk'
// Returns file sector local id in LIFS
uint32_t convert_file(const char* file, const char* disk, uint32_t partition,
    uint32_t previous, uint32_t parent, uint32_t flags, lifs_bitmap_t* bitmap);

// Writes file and file data to disk partition with LIFS
// Returns error code
int write_file(lifs_file_t* header, const char* disk, uint32_t partition, 
    const char* file);

#endif
