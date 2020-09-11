// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_HEADER_H__
#define __LIFS_HEADER_H__

#include <stdint.h>

#include <lifs_bitmap.h>

#define _LIFS_SIGNATURE_ 0x5346494C // "LIFS"
#define _LIFS_VERSION_ 0x302E3120 // "1.0"
#define _LIFS_HEADER_SIZE_ 32

// This struct describes LIFS header image for disk
typedef struct 
{
    uint32_t lifs_jump; // Reserved for boot loader code
    uint32_t lifs_signature; // Use macro
    uint32_t lifs_version; // Use macro
    uint32_t lifs_size; // Size of file system in sectors
    uint32_t lifs_bitmap; // First sector of bitmap
    uint32_t lifs_bitmap_size; // Bitmap size in sectors
    uint32_t lifs_partition; // First partition header sector
    uint32_t lifs_uid; // UID of this file system
} lifs_header_t;

// Creates LIFS header image for disk with specific size
// Returns pointer to LIFS header image
lifs_header_t* 
    create_header(uint32_t disk_size, lifs_bitmap_t* bitmap);

// Writes data from LIFS header image 'header' to disk 'disk'
// Returns error code
int write_header(const char* disk, lifs_header_t* header);

#endif
