// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <time.h>

#include <lifs_commons.h>

uint32_t find_first_free_sector(lifs_bitmap_t* bitmap)
{
    uint32_t i = 0;

    while(bitmap->bytes[i] == UINT8_MAX)
    {
        if(i > bitmap->size)
        {
            return 0;
        }

        i++;
    }

    uint8_t j = 0; 

    while(bitmap->bytes[i] & (1 << j))
    {
        j++;
    }

    return i * _LIFS_BITMAP_SECTORS_PER_BYTE_ + j;
}

uint32_t get_time_fts()
{
    return _FLIX_TIMESTAMP_BASE_ + time(NULL);
}
