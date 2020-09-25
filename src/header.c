// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lifs_commons.h>
#include <lifs_bitmap.h>
#include <lifs_header.h>

uint32_t set_uid()
{
    srand(time(NULL) - (rand() << __CHAR_BIT__));

    return (rand() - rand() + rand()) | 
        ((rand() % UINT16_MAX) << sizeof(uint16_t) * __CHAR_BIT__);
}

lifs_header_t* create_header(uint32_t size, uint32_t start, 
    uint32_t label, uint32_t flags, uint32_t previous, uint32_t next)
{
    if(size < _LIFS_MIN_FS_SIZE_)
    {
        return 0;
    }

    static lifs_header_t header;
    
    header.uid_hi = set_uid();
    header.uid_lo = set_uid();
    header.signature = _LIFS_SIGNATURE_;
    header.version = _LIFS_VERSION_;
    header.sector = start;
    header.size = size;
    header.flags = flags;
    header.bitmap_start = 1;
    header.bitmap_size = get_bitmap_size_s(size);
    header.bsdata = 0;
    header.content = header.bitmap_start + header.bitmap_size;
    header.label = label;
    header.mount = 0;
    header.sys = 0;
    header.previous = previous;
    header.next = next;

    for(int i = 0; i < _LIFS_HEADER_RESERVED_SIZE_; i++)
    {
        header.reserved[i] = 0;
    }

    return &header;
}

int write_header(const char* disk, lifs_header_t* header)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return 1;
    }

    fseek(dd, header->sector * _LIFS_SECTOR_SIZE_, 0);
    fwrite(header, _LIFS_HEADER_SIZE_, 1, dd);

    fclose(dd);

    return 0;
}
