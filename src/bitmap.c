// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <malloc.h>
#include <stdio.h>

#include <lifs_commons.h>
#include <lifs_bitmap.h>

uint32_t get_bitmap_size_s(uint32_t fs_size)
{
    uint32_t byte_size = get_bitmap_size_b(fs_size);

    return byte_size % _LIFS_SECTOR_SIZE_ == 0 ? 
        byte_size / _LIFS_SECTOR_SIZE_ : 
        byte_size / _LIFS_SECTOR_SIZE_ + 1;
}

uint32_t get_bitmap_size_b(uint32_t fs_size)
{
    return fs_size % _LIFS_BITMAP_SECTORS_PER_BYTE_ == 0 ? 
        fs_size / _LIFS_BITMAP_SECTORS_PER_BYTE_ : 
        fs_size / _LIFS_BITMAP_SECTORS_PER_BYTE_ + 1;
}

void clear_bitmap(lifs_bitmap_t* bitmap)
{
    free(bitmap->bytes);
}

lifs_bitmap_t* create_bitmap(uint32_t fs_size, uint32_t start)
{
    if(fs_size < _LIFS_MIN_FS_SIZE_)
    {
        return 0;
    }

    static lifs_bitmap_t bitmap;

    bitmap.sector = start;
    bitmap.size = get_bitmap_size_b(fs_size);
    bitmap.bytes = malloc(bitmap.size);

    for(int i = 0; i < bitmap.size; i++)
    {
        bitmap.bytes[i] = 0;
    }

    for(int i = 0; i < get_bitmap_size_s(fs_size) + 1; i++)
    {
        bitmap.bytes[i / _LIFS_BITMAP_SECTORS_PER_BYTE_] |= 
            1 << (i % _LIFS_BITMAP_SECTORS_PER_BYTE_);
    }

    return &bitmap;
}

int bitmap_mark_sector(lifs_bitmap_t* bitmap, uint32_t sector, uint8_t mark)
{
    if(bitmap->size < sector / _LIFS_BITMAP_SECTORS_PER_BYTE_)
    {
        return -1;
    }

    if(mark == _LIFS_BITMAP_MARK_FREE_)
    {
        bitmap->bytes[sector / _LIFS_BITMAP_SECTORS_PER_BYTE_] &=
            (UINT8_MAX - (1 << sector % 
            _LIFS_BITMAP_SECTORS_PER_BYTE_));
    }
    else
    {
        bitmap->bytes[sector / _LIFS_BITMAP_SECTORS_PER_BYTE_] |=
            1 << (sector % _LIFS_BITMAP_SECTORS_PER_BYTE_);
    }

    return 0;
}

int update_bitmap(const char* disk, lifs_bitmap_t* bitmap)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd < 0)
    {
        return -1;
    }

    fseek(dd, bitmap->sector * _LIFS_SECTOR_SIZE_, 0);
    fwrite(bitmap->bytes, bitmap->size, 1, dd);

    uint8_t last_byte = bitmap->size % _LIFS_SECTOR_SIZE_;

    if(last_byte != 0)
    {
        for(int i = 0; i < _LIFS_SECTOR_SIZE_ - last_byte; i++)
        {
            fputc(0, dd);
        }
    }

    if(fseek(dd, 0, SEEK_CUR) % _LIFS_SECTOR_SIZE_ != 0)
    {
        fclose(dd);

        return 1;
    }

    fclose(dd);

    return 0;
}
