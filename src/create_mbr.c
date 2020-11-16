// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool.

#include <stdio.h>

#include <lifs_diskcrt.h>

#define MBR_SIG 0x55AA

int create_mbr(const char* disk, const char* mbr_img)
{
    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        printf("\033[0;31mERROR\033[0m: Disk access denied!\n\n\n");

        return 1;
    }

    FILE* fd = fopen(mbr_img, "rb");

    if(fd == NULL)
    {
        printf("\033[0;31mERROR\033[0m: Can't open MBR image!\n\n\n");

        fclose(dd);

        return 1;
    }

    uint8_t* buffer = NULL;

    if(fread(buffer, _LIFS_SECTOR_SIZE_, 1, fd) != _LIFS_SECTOR_SIZE_)
    {
        fclose(fd);
        fclose(dd);

        printf("\033[0;31mERROR\033[0m: MBR image is damaged!\n\n\n");

        return 1;
    }

    if(!((buffer[_LIFS_SECTOR_SIZE_ - 1 - 1] == MBR_SIG >> __CHAR_BIT__) &&
        buffer[_LIFS_SECTOR_SIZE_ - 1] == MBR_SIG & UINT8_MAX))
    {
        fclose(fd);
        fclose(dd);

        printf("\033[0;31mERROR\033[0m: MBR image is damaged!\n\n\n");

        return -1;
    }

    if(fwrite(buffer, _LIFS_SECTOR_SIZE_, 1, dd) != _LIFS_SECTOR_SIZE_)
    {
        fclose(dd);
        fclose(fd);

        printf("\033[0;31mERROR\033[0m: Buffer is damaged!\n\n\n");

        return -255;
    }

    fclose(fd);
    fclose(dd);

    return 0;
}
