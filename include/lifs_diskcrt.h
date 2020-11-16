// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool.

#ifndef __LIFS_DISKCRT_H__
#define __LIFS_DISKCRT_H__

#include <lifs_commons.h>
#include <lifs_directory.h>
#include <lifs_header.h>

int partitioning(uint8_t p_offset, const char* disk, uint32_t size, 
    const char* config);

int create_mbr(const char* disk, const char* mbr_img);

#endif
