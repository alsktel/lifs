// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_HEADER_H__
#define __LIFS_HEADER_H__

#include <stdint.h>

#include <lifs_bitmap.h>

#define _LIFS_SIGNATURE_ 0x5346494C // "LIFS"
#define _LIFS_VERSION_ 0x302E3120 // "1.0"
#define _LIFS_HEADER_BOOTCODE_SIZE_ 3
#define _LIFS_HEADER_SIZE_ 64

// This struct describes LIFS header image for disk
typedef struct 
{
    uint8_t boot_code[_LIFS_HEADER_BOOTCODE_SIZE_]; // Boot loader jump code
    uint8_t disk_id; // Id of disk on which this header is
    uint32_t lifs_signature; // Use macro
    uint32_t lifs_version; // Use macro
    uint32_t lifs_size; // Size of file system in sectors
    uint32_t lifs_sector; // This header id, 0 sector for all data in this FS
    uint32_t lifs_bitmap; // First sector of bitmap
    uint32_t lifs_bitmap_size; // Bitmap size in sectors
    uint32_t lifs_content; // First file header sector
    uint32_t lifs_uid_hi; // First part of this file system UID
    uint32_t lifs_uid_lo; // Second part of this file system UID
    uint32_t lifs_part_label; // Partition label (4 chars code)
    uint32_t lifs_flags; // Each bit is a flag
    uint32_t lifs_mount; // Sector with header of directory, handles this FS
    uint32_t lifs_previous; // Previos LIFS header on this disk
    uint32_t lifs_next; // Next LIFS header on this disk
    uint32_t lifs_ext_data; // Sector with other parameters for this FS 
} lifs_header_t;

// Creates LIFS header image with specific size, that starts at 'start' sector
// with specific label (NOTE! Use macroses for labels)
// Returns pointer to LIFS header image
lifs_header_t* create_header(uint32_t size, uint32_t start, 
    uint32_t label, uint32_t flags, uint32_t previous, uint32_t next);

// Writes data from LIFS header image 'header' to disk 'disk'
// Returns error code
int write_header(const char* disk, lifs_header_t* header);

#endif
