// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lifs_header.h>

uint32_t set_uid()
{
    srand(time(NULL) - (rand() << __CHAR_BIT__));

    return (rand() - rand() + rand()) | 
        ((rand() % UINT16_MAX) << sizeof(uint16_t) * __CHAR_BIT__);
}

lifs_header_t* 
    create_header(uint32_t disk_size, lifs_bitmap_t* bitmap)
{
    if(disk_size < 5)
    {
        return 0;
    }

    static lifs_header_t header;

    header.lifs_jump = 0;
    header.lifs_signature = _LIFS_SIGNATURE_;
    header.lifs_version = _LIFS_VERSION_;
    header.lifs_size = disk_size;
    header.lifs_bitmap = _LIFS_BITMAP_FIRST_SECTOR_;
    header.lifs_bitmap_size = get_bitmap_size_s(disk_size);
    header.lifs_partition = 
        bitmap->size + _LIFS_BITMAP_FIRST_SECTOR_;
    header.lifs_uid = set_uid();

    return &header;
}

int write_header(const char* disk, lifs_header_t* header)
{
    FILE* file = fopen(disk, "r+b");

    if(file == NULL)
    {
        return 1;
    }

    fscanf(file, "%d", &(header->lifs_jump));

    printf("%d\n", header->lifs_bitmap);

    fseek(file, 0, SEEK_SET);

    fwrite(header, _LIFS_HEADER_SIZE_, 1, file);

    fclose(file);

    return 0;
}
