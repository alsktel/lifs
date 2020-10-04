// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <time.h>
#include <malloc.h>
#include <string.h>

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

int convert_file_name(const char* name, uint8_t* converted)
{
    uint16_t len = 0;

    while(name[len] != '\0')
    {
        if(len > UINT8_MAX)
        {
            return -1;
        }

        len++;
    }

    for(int i = 0; i < len + 1; i++)
    {
        converted[i] = name[i];
    }

    for(int i = len + 1; i < _LIFS_NAMES_LEN_; i++)
    {
        converted[i] = 0;
    }

    return 0;
}

char* get_file_name(char* path)
{
    char sep[3] = "/\\";
    char* token;
    char* name;

    token = strtok(path, sep);

    if(token == NULL)
    {
        return path;
    }

    do
    {
        name = token;
        token = strtok(NULL, sep);
    }
    while(token != NULL);

    return name;
}
