// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lifs_commons.h>
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
    if(size < 5)
    {
        return 0;
    }

    static lifs_header_t header;

    for(int i = 0; i < _LIFS_HEADER_BOOTCODE_SIZE_; i++)
    {
        header.boot_code[i] = 0;
    }

    header.disk_id = 0;
    header.lifs_signature = _LIFS_SIGNATURE_;
    header.lifs_version = _LIFS_VERSION_;
    header.lifs_size = size;
    header.lifs_sector = start;
    header.lifs_bitmap = _LIFS_BITMAP_FIRST_SECTOR_;
    header.lifs_bitmap_size = get_bitmap_size_s(size);
    header.lifs_content = header.lifs_bitmap_size + _LIFS_BITMAP_FIRST_SECTOR_;
    header.lifs_uid_hi = set_uid();
    header.lifs_uid_lo = set_uid();
    header.lifs_part_label = label;
    header.lifs_flags = flags;
    header.lifs_mount = 0;
    header.lifs_previous = previous;
    header.lifs_next = next;
    header.lifs_ext_data = 0;

    return &header;
}

int write_header(const char* disk, lifs_header_t* header)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return 1;
    }

    fseek(dd, header->lifs_sector * _LIFS_SECTOR_SIZE_, 0);
    fread(header, _LIFS_HEADER_BOOTCODE_SIZE_ + 1, 1, dd);

    fseek(dd, header->lifs_sector * _LIFS_SECTOR_SIZE_, 0);
    fwrite(header, _LIFS_HEADER_SIZE_, 1, dd);

    fclose(dd);

    return 0;
}
