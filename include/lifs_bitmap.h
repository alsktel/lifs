// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_BITMAP_H__
#define __LIFS_BITMAP_H__

#include <stdint.h>

#define _LIFS_BITMAP_FIRST_SECTOR_ 2
#define _LIFS_BITMAP_SECTORS_PER_BYTE_ 8
#define _LIFS_BITMAP_MARK_FREE_ 0
#define _LIFS_BITMAP_MARK_USED_ 1

// This struct describes LIFS sectors bitmap image
typedef struct
{
    uint8_t* bytes; // Each byte contains flags for 8 sectors
    uint32_t size; // Size in bytes
} lifs_bitmap_t;

// Creates bitmap image for LIFS
// Returns pointer to bitmap 
lifs_bitmap_t* create_bitmap(uint32_t disk_size);

// Marks specific sector field in bitmap image 'bitmap'
// with value from 'mark'
// NOTICE: Use macroses defined in this file (_LIFS_BITMAP_MARK_*) 
// as values of 'mark' 
// Returns error code
int bitmap_mark_sector(lifs_bitmap_t* bitmap, 
    uint32_t sector, uint8_t mark);

// Updates bitmap sectors on disk 'disk' with data 
// from bitmap image 'bitmap' 
// Returns error code
int update_bitmap(const char* disk, lifs_bitmap_t* bitmap);

// Returns bitmap size in sectors
uint32_t get_bitmap_size_s(uint32_t disk_size);

// Returns bitmap size in bytes
uint32_t get_bitmap_size_b(uint32_t disk_size);

// Clears bitmap image, frees allocated memory
void clear_bitmap(lifs_bitmap_t* bitmap);

#endif
