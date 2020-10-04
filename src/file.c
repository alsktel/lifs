// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <malloc.h>

#include <lifs_file.h>

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

uint32_t create_nodes(lifs_file_node_t** nodes, 
    uint32_t size, lifs_bitmap_t* bitmap)
{
    if (*nodes != NULL)
    {
        *nodes = NULL;
    }

    uint32_t count = size % _LIFS_NODE_DATA_COUNT_ == 0 ? 
        size / _LIFS_NODE_DATA_COUNT_ : size / _LIFS_NODE_DATA_COUNT_ + 1;

    *nodes = malloc(_LIFS_SECTOR_SIZE_ * count);

    for(int i = 0; i < count; i++)
    {
        for(int j = 0; j < _LIFS_NODE_DATA_COUNT_; j++)
        {
            (*nodes)[i].sectors[j] = 0;
        }
    }

    uint32_t start = find_first_free_sector(bitmap);

    for(int i = 0; i < count; i++)
    {
        (*nodes)[i].next = start + i + 1;
        (*nodes)[i].previous = start + i - 1;

        for(int j = 0; j < _LIFS_NODE_DATA_COUNT_; j++)
        {
            if(i * _LIFS_NODE_DATA_COUNT_ + j == size)
            {
                break;
            }

            (*nodes)[i].sectors[j] = start + count + j;
        }

        if(bitmap_mark_sector(bitmap, start + i, _LIFS_BITMAP_MARK_USED_))
        {
            return 0;
        }
    }

    (*nodes)[0].previous = 0;
    (*nodes)[count - 1].next = 0;

    return count;
}

int write_file(lifs_file_t* header, lifs_file_node_t* nodes, uint32_t count,
    const char* disk, uint32_t partition, const char* file)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return - 1;
    }

    FILE* fd = fopen(file, "rb");

    if(fd == NULL)
    {
        return - 1;
    }

    fseek(dd, (partition + header->sector) * _LIFS_SECTOR_SIZE_, 0);
    fwrite(header, _LIFS_SECTOR_SIZE_, 1, dd);
    fwrite(nodes, _LIFS_SECTOR_SIZE_, count, dd);

    int byte;

    while((byte = fgetc(fd)) != EOF)
    {
        fputc(byte, dd);
    }

    fputc(_LIFS_FILE_EOF_, dd);

    fclose(dd);
    fclose(fd);

    return 0;
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

    lifs_file_node_t* nodes = NULL;

    uint32_t nodes_count = create_nodes(&nodes, size, bitmap);

    if(write_file(header, nodes, nodes_count, disk, partition, file))
    {
        return 0;
    }

    for(int i = 0; i < size; i++)
    {
        bitmap_mark_sector(bitmap, header->content + nodes_count + i, 
            _LIFS_BITMAP_MARK_USED_);
    }

    update_bitmap(disk, bitmap);

    free(nodes);

    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        return 0;
    }

    fseek(dd, (partition + previous) * _LIFS_SECTOR_SIZE_, 0);
    fseek(dd, _LIFS_FILE_NEXT_FIELD_OFFSET_, SEEK_CUR);
    fwrite((void*)header + _LIFS_FILE_SECTOR_FIELD_OFFSET_, 
        sizeof(uint32_t), 1, dd);

    fclose(dd);

    return header->sector;
}
