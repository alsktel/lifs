// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool.

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include <lifs_diskcrt.h>

uint32_t to_sect(char* size)
{
    switch(size[strlen(size) - 1])
    {
        default: return 0;
        case 'K': return atoi(size) * 2;
        case 'M': return atoi(size) * 2 * 1024;
        case 'G': return atoi(size) * 2 * 1024 * 1024;
    }
}

int u_part_loop(const char* disk, uint32_t size, uint32_t start, uint32_t prev)
{
    static uint8_t i = 0;
    char answer[4];
    static int root_f = 0;

    if(prev != 0)
    {
        i++;
    }

    do
    {
        printf("\n\nCreating partition #%d\n\n", i);
        printf("Select partition type [1 - root/2 - data]: ");

        uint8_t p_type;

        scanf("%d", &p_type);

        if(p_type > 2)
        {
            printf("[\033[0;31mERROR\033[0m]: Invalid partition type!\n");

            continue;
        }

        if(root_f == 1 && p_type == 1)
        {
            printf("\n[\033[0;31mERROR\033[0m]: ");
            printf("Root partition is already exit!\n");

            continue;
        }

        uint32_t p_size;
        char psz[UINT8_MAX];

        printf("Enter partition size: ");
        scanf("%s", &psz);

        p_size = to_sect(psz);

        if(p_size == 0)
        {
            continue;
        }

        if(p_size + 1 + get_bitmap_size_s(p_size) > size)
        {
            printf("\n[\033[0;31mERROR\033[0m]: Partition is too large!\n");

            continue;
        }

        char root[_LIFS_NAMES_LEN_];

        printf("Enter path too directory, which contains content "); 
        printf("for this partition: ");
        scanf("%s", &root);

        if(p_type == 1)
        {
            root_f = 1;

            prev = create_lifs(disk, prev, start, p_size, 
                _LIFS_LABEL_ROOT_, root);

            if(prev == 0)
            {
                printf("\n[\033[0;31mERROR\033[0m]: ");
                printf("Can't create partition, tool crashed!\n");

                return 1;
            }
        }
        else
        {
            prev = create_lifs(disk, prev, start, p_size, 
                _LIFS_LABEL_DATA_, root);

            if(prev == 0)
            {
                printf("\n[\033[0;31mERROR\033[0m]: %s");
                printf("Can't create partition, tool crashed!\n");

                return 1;
            }
        }

        start += p_size + 1 + get_bitmap_size_s(p_size);
        size -= p_size + 1 + get_bitmap_size_s(p_size);

        printf("\nPartition created\033[0;32m successfully\033[0m!\n");
        printf("Disk free space: %d sectors (%d bytes)\n\n", size, size * 512);
        printf("Create new partition? [Y/N]: ");
        scanf("%s", &answer);

        ++i;

    } while(!(strcmp(answer, "Yes") && strcmp(answer, "YES") && 
        strcmp(answer, "Y") && strcmp(answer, "y")));

    return 0;
}

int user_part(uint8_t p_offset, const char* disk, uint32_t size)
{
    FILE* dd = fopen(disk, "r+b");

    fseek(dd, sizeof(uint16_t) + sizeof(uint8_t), 0);
    fputc(p_offset, dd);

    printf("Chose wether this disk is system (need boot partition) or not.\n");
    printf("Create BOOT partition? [Y/N]: ");

    char answer[4];

    scanf("%s", &answer);

    uint32_t st = p_offset;
    uint32_t prev = 0;

    while(!(strcmp(answer, "Yes") && strcmp(answer, "YES") && 
        strcmp(answer, "Y") && strcmp(answer, "y")))
    {
        printf("\nEnter partition size: ");

        char psz[UINT8_MAX];

        scanf("%s", &psz);

        uint32_t psize = to_sect(psz);

        if(psize == 0)
        {
            printf("\033[0;31mERROR\033[0m: Invalid partition size!\n");

            continue;
        }

        if(psize + 1 + get_bitmap_size_s(psize) > size)
        {
            printf("\033[0;31mERROR\033[0m: Partition is too large!\n");

            continue;
        }

        printf("Enter directory from which LIFS partition will be created: ");

        char root[_LIFS_NAMES_LEN_];

        scanf("%s", &root);

        prev = create_lifs(disk, prev, st, psize, _LIFS_LABEL_BOOT_, root);

        if(prev == 0)
        {
            printf("\n[\033[0;31mERROR\033[0m]: Can't create partition!\n\n");

            return 1;
        }

        size -= psize + 1 + get_bitmap_size_s(psize);
        st += psize + 1 + get_bitmap_size_s(psize);

        printf("\nPartition created\033[0;32m successfully\033[0m!\n");
        printf("Disk free space: %d sectors (%d bytes)\n\n", size, size * 512);

        break;
    }

    printf("Create new partition? [Y/N]: ");
    scanf("%s", &answer);

    if(!(strcmp(answer, "Yes") && strcmp(answer, "YES") && 
        strcmp(answer, "Y") && strcmp(answer, "y")))
    {
        return u_part_loop(disk, size, st, prev);
    }

    return 0;
}

int conf_part(const char* config, const char* disk, uint32_t size)
{

}

int partitioning(uint8_t p_offset, const char* disk, uint32_t size, 
    const char* config)
{
    if(config == NULL)
    {
        if(user_part(p_offset, disk, size))
        {
            return 1;
        }

        return 0;
    }

    if(conf_part(config, disk, size))
    {
        return 1;
    }

    return 0;
}
