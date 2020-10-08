// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <malloc.h>

#include <lifs_file.h>

#define LIFS_CHARS_PER_SECTOR 504

lifs_file_t* create_file(const char* name, uint32_t flags, uint32_t size, 
    uint32_t sector, uint32_t content, uint32_t parent, uint32_t previous)
{
    static lifs_file_t header;

    if(convert_file_name(name, header.name))
    {
        return 0;
    }

    header.link = 0;
    header.size = size;
    header.flags = flags;
    header.content = content;
    header.sector = sector;
    header.parent = parent;
    header.previous = previous;
    header.next = 0;
    header.additional = 0;
    header.created = get_time_fts();
    header.edited = header.created;
    header.accessed = header.edited;

    for(int i = 0; i < _LIFS_FILE_RESERVED_SIZE_; i++)
    {
        header.reserved[i] = 0;
    }

    return &header;
}

int write_file(lifs_file_t* header, const char* disk, uint32_t partition, 
    const char* file)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return -1;
    }

    FILE* fd = fopen(file, "rb");

    if(fd == NULL)
    {
        return -1;
    }

    int rval = 0;

    fseek(dd, (partition + header->sector) * _LIFS_SECTOR_SIZE_, 0);
    fwrite(header, _LIFS_SECTOR_SIZE_, 1, dd);
    fputc(0, dd);
    fputc(0, dd);
    fputc(0, dd);
    fputc(0, dd);

    int byte;
    uint32_t i = 0;

    while((byte = fgetc(fd)) != EOF)
    {
        if(i % LIFS_CHARS_PER_SECTOR == 0 && i != 0)
        {
            fputc((header->sector + 1 + 
                (i / LIFS_CHARS_PER_SECTOR)) & UINT8_MAX, dd);
            fputc(((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >>
                (sizeof(uint8_t) * __CHAR_BIT__)) & UINT8_MAX, dd);
            fputc((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >> 
                (sizeof(uint16_t) * __CHAR_BIT__), dd);
            fputc(((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >>
                ((sizeof(uint16_t) + 1)* __CHAR_BIT__)), dd);
            fputc((header->sector + 
                (i / LIFS_CHARS_PER_SECTOR)) & UINT8_MAX, dd);
            fputc(((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >>
                (sizeof(uint8_t) * __CHAR_BIT__)) & UINT8_MAX, dd);
            fputc((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >> 
                (sizeof(uint16_t) * __CHAR_BIT__), dd);
            fputc(((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >>
                ((sizeof(uint16_t) + 1)* __CHAR_BIT__)), dd);
        }

        fputc(byte, dd);

        i++;
    }

    if(i % LIFS_CHARS_PER_SECTOR == 0)
    {
        rval = 1;

        fputc((header->sector + 1 + 
            (i / LIFS_CHARS_PER_SECTOR)) & UINT8_MAX, dd);
        fputc(((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >>
            (sizeof(uint8_t) * __CHAR_BIT__)) & UINT8_MAX, dd);
        fputc((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >> 
            (sizeof(uint16_t) * __CHAR_BIT__), dd);
        fputc(((header->sector + 1 + (i / LIFS_CHARS_PER_SECTOR)) >>
            ((sizeof(uint16_t) + 1)* __CHAR_BIT__)), dd);
        fputc((header->sector + 
            (i / LIFS_CHARS_PER_SECTOR)) & UINT8_MAX, dd);
        fputc(((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >>
            (sizeof(uint8_t) * __CHAR_BIT__)) & UINT8_MAX, dd);
        fputc((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >> 
            (sizeof(uint16_t) * __CHAR_BIT__), dd);
        fputc(((header->sector + (i / LIFS_CHARS_PER_SECTOR)) >>
            ((sizeof(uint16_t) + 1)* __CHAR_BIT__)), dd);
        fputc(_LIFS_FILE_EOF_, dd);

        for(int i = 0; i < _LIFS_SECTOR_SIZE_ - sizeof(uint32_t) - 1; i++)
        {
            fputc(0, dd);
        }
    }
    else
    {
        fputc(_LIFS_FILE_EOF_, dd);
    }

    fclose(dd);
    fclose(fd);

    return rval;
}

uint32_t convert_file(const char* file, const char* disk, uint32_t partition,
    uint32_t previous, uint32_t parent, uint32_t flags, lifs_bitmap_t* bitmap)
{
    uint32_t sector = find_first_free_sector(bitmap);

    FILE* fd = fopen(file, "rb");

    if(fd == NULL)
    {
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    
    uint32_t size = (ftell(fd) + 1) % _LIFS_SECTOR_SIZE_ == 0 ? 
        (ftell(fd) + 1) / _LIFS_SECTOR_SIZE_ : (ftell(fd) + 1) / 
        _LIFS_SECTOR_SIZE_ + 1;

    fclose(fd);

    lifs_file_t* header = create_file(get_file_name(file), flags, size, 
        sector, size > 0 ? sector + 1 : 0, parent, previous);

    if(header == NULL)
    {
        return 0;
    }

    bitmap_mark_sector(bitmap, header->sector, _LIFS_BITMAP_MARK_USED_);

    if(size > 0)
    {
        int wfr = write_file(header, disk, partition, file);

        if(wfr == 1)
        {
            size++;
        }
        else if(wfr != 0)
        {
            return 0;
        }

        for(int i = 0; i < size; i++)
        {
            bitmap_mark_sector(bitmap, header->content + i, 
                _LIFS_BITMAP_MARK_USED_);
        }
    }
    else
    {
        FILE* dd = fopen(disk, "r+b");

        if(dd == NULL)
        {
            return 0;
        }

        fseek(dd, header->sector * _LIFS_SECTOR_SIZE_, 0);
        fwrite(header, _LIFS_SECTOR_SIZE_, 1, dd);

        fclose(dd);
    }

    update_bitmap(disk, bitmap);

    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return 0;
    }

    if(previous != 0)
    {
        fseek(dd, (partition + previous) * _LIFS_SECTOR_SIZE_, 0);
        fseek(dd, _LIFS_FILE_NEXT_FIELD_OFFSET_, SEEK_CUR);
        fwrite((void*)header + _LIFS_FILE_SECTOR_FIELD_OFFSET_, 
            sizeof(uint32_t), 1, dd);
    }

    fclose(dd);

    return header->sector;
}
