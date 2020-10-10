// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_HEADER_H__
#define __LIFS_HEADER_H__

#include <stdint.h>

#include <lifs_bitmap.h>

#define _LIFS_SIGNATURE_ 0x5346494C // "LIFS"
#define _LIFS_VERSION_ 0x302E3120 // "1.0"
#define _LIFS_HEADER_MAIN_SIZE_ 64
#define _LIFS_HEADER_SIZE_ 512
#define _LIFS_HEADER_RESERVED_SIZE_ 512 - 64
#define _LIFS_LABEL_BOOT_ 0x544F4F42
#define _LIFS_LABEL_ROOT_ 0x544F4F52
#define _LIFS_LABEL_DATA_ 0x41544144
#define _LIFS_HEADER_NEXT_FIELD_ 60
#define _LIFS_HEADER_SECTOR_FIELD_ 16

// This struct describes LIFS header
typedef struct 
{
    uint32_t signature; // LIFS
    uint32_t version; // Version format: <space> Major.Minor in UTF-8
    uint32_t bsdata; // Reserved for boot system data
    uint32_t label; // 4 chars label
    uint32_t sector; // Global id of sector with this header
    uint32_t size; // Size of this FS in sectors
    uint32_t flags; // FS flags (see docs)
    uint32_t content; // Local id of sector with first file in this FS
    uint32_t bitmap_start; // Local id of first bitmap sector
    uint32_t bitmap_size; // Bitmap size in sectors
    uint32_t uid_hi; // Higher 32 bit of UID (THIS-THIS-xxxx-xxxx)
    uint32_t uid_lo; // Lower 32 bit of UID (xxxx-xxxx-THIS-THIS)
    uint32_t sys; // Reserved for OS data 
    uint32_t mount; // Global id of sector with this FS mount directory
    uint32_t previous; // Global id of sector with previous FS header
    uint32_t next; // Global id of sector with previous FS header 
    uint8_t reserved[_LIFS_HEADER_RESERVED_SIZE_]; // Reserved (see docs)
} lifs_header_t;

// Creates LIFS header image with specific size, that starts at 'start' sector
// with specific label (NOTE! Use macroses for labels)
// Returns pointer to LIFS header image
lifs_header_t* create_header(uint32_t size, uint32_t start, 
    uint32_t label, uint32_t flags, uint32_t previous);

// Writes data from LIFS header image 'header' to disk 'disk'
// Returns error code
int write_header(const char* disk, lifs_header_t* header);

#endif
