// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS crestion tool

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <lifs_file.h>
#include <lifs_header.h>

#define BOOT_FLAGS 0b00000000000000000000000000000001
#define ROOT_FLAGS 0b00000000000000000000000000000010
#define DATA_FLAGS 0b00000000000000000000000000000100

uint32_t convert_dir(const char* path, const char* disk, uint32_t partition,
    uint32_t previous, uint32_t parent, lifs_bitmap_t* bitmap)
{
    DIR* dir = opendir(path);

    if(dir == NULL)
    {
        printf("%s <%s>\n", "[\033[0;31mERROR\033[0m]: Can't open", path);

        return 0;
    }

    struct dirent* ent;
    uint32_t ent_count = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(strcmp(ent->d_name, ".") != 0 
            && strcmp(ent->d_name, "..") != 0)
        {
            ent_count++;
        }
    }

    char* name = get_file_name(path);

    lifs_file_t* this_dir = create_file(name, 
        _LIFS_DIRECTORY_ATTRIBUTES_DEFAULT_, ent_count, 
        find_first_free_sector(bitmap), ent_count > 0 ? 
        find_first_free_sector(bitmap) + 1 : 0, parent, previous);

    free(name);

    uint32_t rval = this_dir->sector;

    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        printf("%s\n", "[\033[0;31mERROR\033[0m]: Can't open disk!");

        return 0;
    }

    fseek(dd, (partition + this_dir->sector) * _LIFS_SECTOR_SIZE_, 0);
    fwrite(this_dir, _LIFS_SECTOR_SIZE_, 1, dd);

    if(previous != 0)
    {
        fseek(dd, (partition + this_dir->previous) * _LIFS_SECTOR_SIZE_, 0);
        fseek(dd, _LIFS_FILE_NEXT_FIELD_OFFSET_, SEEK_CUR);
        fwrite((void*)this_dir + _LIFS_FILE_SECTOR_FIELD_OFFSET_, 
        sizeof(uint32_t), 1, dd);
    }

    fclose(dd);

    bitmap_mark_sector(bitmap, this_dir->sector, _LIFS_BITMAP_MARK_USED_);
    update_bitmap(disk, bitmap);

    seekdir(dir, 0);

    uint32_t prev = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_type != DT_DIR)
        {
            char f_path[strlen(path) + 1 + _LIFS_NAMES_LEN_];

            strcpy(f_path, path);
            strcat(f_path, "/");
            strcat(f_path, ent->d_name);

            prev = convert_file(f_path, disk, partition, prev, rval, 
                _LIFS_FILE_ATTRIBUTES_DEFAULT_, bitmap);

            if(prev == 0)
            {
                printf("%s '%s'\n", 
                    "[\033[0;31mERROR\033[0m]: Can't open file: ", f_path);
            }
        }
    }

    seekdir(dir, 0);

    prev = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 
            && strcmp(ent->d_name, "..") != 0)
        {
            char d_path[strlen(path) + 1 + _LIFS_NAMES_LEN_];

            strcpy(d_path, path);
            strcat(d_path, "/");
            strcat(d_path, ent->d_name);

            prev = convert_dir(d_path, disk, partition, prev, rval, bitmap);

            if(prev == 0)
            {
                return 0;
            }
        }
    }

    closedir(dir);

    return rval;
}

uint32_t create_lifs(const char* disk, uint32_t previous, uint32_t start,
    uint32_t size, uint32_t label, const char* root)
{
    uint32_t f = 0;

    switch(label)
    {
        default: break;
        case _LIFS_LABEL_BOOT_: f = BOOT_FLAGS; break;
        case _LIFS_LABEL_ROOT_: f = ROOT_FLAGS; break;
        case _LIFS_LABEL_DATA_: f = DATA_FLAGS; break;
    }

    lifs_header_t* header = create_header(size, start, label, f, previous);
    lifs_bitmap_t* bitmap = create_bitmap(size, 
        header->bitmap_start + header->sector);

    write_header(disk, header);
    update_bitmap(disk, bitmap);

    uint32_t rval = header->sector;

    DIR* dir = opendir(root);

    if(dir == NULL)
    {
        printf("%s '%s'\n", "[\033[0;31mERROR\033[0m]: Can't open", root);

        return 0;
    }

    struct dirent* ent;
    uint32_t ent_count = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(strcmp(ent->d_name, ".") != 0 
            && strcmp(ent->d_name, "..") != 0)
        {
            ent_count++;
        }
    }

    seekdir(dir, 0);

    uint32_t prev = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_type != DT_DIR)
        {
            char f_path[strlen(root) + 1 + _LIFS_NAMES_LEN_];

            strcpy(f_path, root);
            strcat(f_path, "/");
            strcat(f_path, ent->d_name);

            prev = convert_file(f_path, disk, rval, prev, 0, 
                _LIFS_FILE_ATTRIBUTES_DEFAULT_, bitmap);

            if(prev == 0)
            {
                return 0;
            }
        }
    }

    seekdir(dir, 0);

    prev = 0;

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 
            && strcmp(ent->d_name, "..") != 0)
        {
            char d_path[strlen(root) + 1 + _LIFS_NAMES_LEN_];

            strcpy(d_path, root);
            strcat(d_path, "/");
            strcat(d_path, ent->d_name);

            prev = convert_dir(d_path, disk, rval, prev, 0, bitmap);

            if(prev == 0)
            {
                return 0;
            }
        }
    }

    closedir(dir);

    clear_bitmap(bitmap);

    return rval;
}
