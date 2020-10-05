// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#ifndef __LIFS_DIRECTORY_H__
#define __LIFS_DIRECTORY_H__

#include <lifs_commons.h>

// Converts directory and its contents to LIFS units
// Returns directory header local id in LIFS
uint32_t convert_dir(const char* path, const char* disk, uint32_t partition, 
    uint32_t previous, uint32_t parent, lifs_bitmap_t* bitmap);

// Creates LIFS partition on disk and converts all data from
// 'root' directory to LIFS units on this LIFS partition
// Returns this LIFS partition id
uint32_t create_lifs(const char* disk, uint32_t previous, uint32_t start,
    uint32_t size, uint32_t label, const char* root);

#endif
