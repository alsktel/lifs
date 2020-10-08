// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lifs_header.h>
#include <lifs_directory.h>

#define LIFS_CREATION_TOOL_ARGS_COUNT 5
#define BOOTSIG 0xAA55
#define BOOTLOADERSIG "SXB0"
#define LIFS_START 2

void print_help()
{
    printf("\033[1mUsage:\033[0m mklifs [Disk] [Block size] [Size] {BOOT}\n");
    printf("\n\033[1m[Argument]\t[Description]\033[0m\n");
    printf("Disk\t\tPath to disk image (file)\n");
    printf("Block size\tSpecifies size of block in bytes\n");
    printf("Size\t\tSpecifies disk size (in blocks) to work with\n");
    printf("Boot\t\t1KiB bootloader can be written to disk (see docs)\n\n");
    printf("\033[1mNOTICE:\033[0m You can pass disk size in bytes by using:");
    printf("\n[-s] [X] instead of [Block size] [Size], X - size in bytes");
    printf("\n\n\033[1mLIFS creation stages:\033[0m\n");
    printf("1. Run this tool\n2. Run partitioning:\n");
    printf("3. Select whether you need to add bootloader\n4. Profit\n\n\n");
}

int format_disk(const char* disk, uint32_t size)
{
    printf("\n\033[1;31mWARNING!\033[0m You have passed '");
    printf("%s", disk);
    printf("' as disk for LIFS creation\nin size of ");
    printf("%d sectors (%d bytes).\n", size / _LIFS_SECTOR_SIZE_, size);
    printf("\033[1mThese sectors will be formatted.\033[0m\n");
    printf("It means that\033[1m all data in this file will be ");
    printf("\033[1;31mPERMANENTLY DELETED\033[0m\033[1m\nand it can't be ");
    printf("recovered!\033[0m\n\nAre you sure that you want to continue? ");
    printf("[Yes/No]: ");

    char answer[4];

    scanf("%s", &answer);

    printf("\n");

    if(strcmp(answer, "Yes") && strcmp(answer, "YES") && 
        strcmp(answer, "Y") && strcmp(answer, "y"))
    {
        return 1;
    }

    FILE* dd = fopen(disk, "wb");

    if(dd == NULL)
    {
        return -1;
    }

    fseek(dd, size - 1, 0);
    fputc(0, dd);

    fclose(dd);

    return 0;
}

int write_bootloader(const char* disk, const char* bootloader)
{
    FILE* bd = fopen(bootloader, "r+b");

    if(bd == NULL)
    {
        printf("\n[\033[0;31mERROR\033[0m]: Can't open bootloader file!");

        return -1;        
    }

    fseek(bd, _LIFS_SECTOR_SIZE_ - sizeof(uint16_t), 0);

    if(!(fgetc(bd) | (fgetc(bd) << __CHAR_BIT__) == BOOTSIG))
    {
        fclose(bd);

        printf("\n[\033[0;31mERROR\033[0m]: File is not a bootloader!");

        return UINT8_MAX;
    }

    uint8_t sxb_sig[sizeof(uint32_t) + 1];

    fseek(bd, _LIFS_SECTOR_SIZE_ - sizeof(uint32_t), SEEK_CUR);
    fread(sxb_sig, sizeof(uint32_t), 1, bd);

    sxb_sig[sizeof(uint32_t)] = '\0';

    if(strcmp(sxb_sig, BOOTLOADERSIG))
    {
        fclose(bd);

        printf("\n[\033[0;31mERROR\033[0m]:"); 
        printf("Bootloader is not LIFS compatible! (NO_SXB0)");

        return -UINT8_MAX;
    }

    FILE* dd = fopen(disk, "r+b");

    if(dd == NULL)
    {
        fclose(bd);

        printf("\n[\033[0;31mERROR\033[0m]: Can't open disk!\n\n");

        return -2;
    }

    uint8_t buffer[_LIFS_SECTOR_SIZE_ * 2];

    fseek(bd, 0, 0);
    fread(buffer, _LIFS_SECTOR_SIZE_ * 2, 1, bd);
    fwrite(buffer, _LIFS_SECTOR_SIZE_ * 2, 1, dd);

    fclose(bd);
    fclose(dd);
}

int partitioning(const char* disk, uint32_t disk_size)
{
    uint8_t i = 0;
    uint32_t start = LIFS_START;
    uint32_t prev = 0;
    char answer[4];

    int boot_f = 0;
    int root_f = 0;

    do
    {
        printf("\nCreating partition #%d\n\n", i);
        printf("Select partition type:\n1. Boot partition (Required 1)");
        printf("\n2. Root partition (System partition. Required 1)\n");
        printf("3. Data partition (Optional)\nEnter number: ");

        uint8_t p_type;

        scanf("%d", &p_type);

        uint32_t p_size;

        printf("\nEnter partition size in blocks (each block is 512 bytes): ");
        scanf("%d", &p_size);

        if(disk_size < p_size || disk_size - p_size < 0)
        {
            printf("\n[\033[0;31mERROR\033[0m]: ");
            printf("Partition is too large!\n\nTrying one more time...\n");

            continue;
        }

        char root[_LIFS_NAMES_LEN_];

        printf("Enter path too directory, which contains content "); 
        printf("for this partition: ");
        scanf("%s", &root);

        disk_size -= p_size;

        if(p_type == 1)
        {
            if(boot_f)
            {
                printf("\n[\033[0;31mERROR\033[0m]: ");
                printf("Boot partition is already exit!\n\n");
                printf("Trying one more time...\n");

                continue;
            }

            boot_f = 1;

            prev = create_lifs(disk, prev, start, p_size, 
                _LIFS_LABEL_BOOT_, root);

            if(prev == 0)
            {
                printf("\n[\033[0;31mERROR\033[0m]: ");
                printf("Can't create partition, tool crashed!\n");

                return 1;
            }

            start += p_size;
        }
        else if(p_type == 2)
        {
            if(root_f)
            {
                printf("\n[\033[0;31mERROR\033[0m]: ");
                printf("Root partition is already exit!\n\n");
                printf("Trying one more time...\n");

                continue;
            }

            root_f = 1;

            prev = create_lifs(disk, prev, start, p_size, 
                _LIFS_LABEL_ROOT_, root);

            if(prev == 0)
            {
                printf("\n[\033[0;31mERROR\033[0m]: ");
                printf("Can't create partition, tool crashed!\n");

                return 1;
            }

            start += p_size;
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

            start += p_size;
        }

        printf("\n[\033[0;32mOK\033[0m]: Partition created\n\n");
        printf("Continue with creating of new partition? [Yes/No]: ");
        scanf("%s", &answer);

        i++;

    } while(!(strcmp(answer, "Yes") && strcmp(answer, "YES") && 
        strcmp(answer, "Y") && strcmp(answer, "y")));

    return 0;
}

int main(int argc, char** argv)
{
    printf("\n\n\033[1m%s\033[0m v.", "LIFS creation tool");
    putchar(_LIFS_VERSION_ & UINT8_MAX);
    putchar((_LIFS_VERSION_ >> sizeof(uint8_t) * __CHAR_BIT__) & UINT8_MAX);
    putchar((_LIFS_VERSION_ >> sizeof(uint16_t) * __CHAR_BIT__) & UINT8_MAX);
    putchar(_LIFS_VERSION_ >> ((sizeof(uint16_t) + 1) * __CHAR_BIT__));
    printf("\nCreated by eastev\n<https://github.com/eastev>\n\n");

    if(argc == 1 || !strcmp(argv[1], "-h") || 
        !strcmp(argv[1], "--help"))
    {
        print_help();

        return 0;
    }
    else if(argc > LIFS_CREATION_TOOL_ARGS_COUNT)
    {
        printf("\n[\033[0;31mERROR\033[0m]: %s", "Incorrect arguments!");
        printf(" Use: mklifs -h to see help\n\n");

        return -1;
    }

    const char* disk = argv[1];

    if((!strcmp(argv[2], "-s") && atoi(argv[3]) > UINT32_MAX) ||
        (atoi(argv[2]) * atoi(argv[3])) > UINT32_MAX)
    {
        printf("\n[\033[0;31mERROR\033[0m]: Disk size is not supported\n\n");

        return -2;
    }

    uint32_t size = !strcmp(argv[2], "-s") ? 
        atoi(argv[3]) : atoi(argv[2]) * atoi(argv[3]);

    const char* boot = NULL;

    if(argc == LIFS_CREATION_TOOL_ARGS_COUNT)
    {
        boot = argv[4];
    }

    int dfr = format_disk(disk, size);

    if(dfr == -1)
    {
        printf("\n[\033[0;31mERROR\033[0m]: Can't open disk!\n");

        return 1;
    }
    else if (dfr == 1)
    {
        printf("You have cancelled LIFS creation.\n\n");

        return UINT8_MAX;
    }

    printf("Running partitioning...\n");

    if(partitioning(disk, size))
    {
        return UINT8_MAX / 2;
    }

    printf("\n[\033[0;32mOK\033[0m]: Partitioning successful!\n");

    if(boot != NULL)
    {
        printf("\nAdding bootloader from '%s'...\n", boot);

        if(write_bootloader(disk, boot))
        {
            printf("\n[\033[0;31mERROR\033[0m]: Can't add bootloader!\n\n\n");

            return UINT8_MAX - 1;
        }

        printf("\n[\033[0;32mOK\033[0m]: Bootloader added\n");
    }

    printf("\nFLIX is fully created on '%s'\n\n\n", disk);
    
    return 0;
}
