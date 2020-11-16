// SPDX-License-Identifier: GPL-3.0
// Created by eastev <https://github.com/eastev>

// This file is a part of LIFS creation tool

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lifs_header.h>
#include <lifs_directory.h>
#include <lifs_diskcrt.h>

#define MIN_ARGS 3
#define VERSION "2.1"

void print_help()
{
    printf("\033[1mUsage:\033[0m mklifs {[Option] [Argument] ...} [Disk] ");
    printf("[Size]\n\n");
    printf("\033[1mOption\t\tArgument\tDescription\033[0m\n");
    printf("-h or --help\tNone\t\tShow this help\n");
    printf("-b or --boot\tMBR image\tMake boot sector (MBR) from file\n");
    printf("-c or --conf\tConfig file\tMake LIFS with configs from file\n");
    printf("-s or --size\tFormat\t\tOther format of size, see below\n");
    printf("-r or --skip\tSize\t\tSkip some place from disk start\n");
    printf("\n\033[1mOther format of size\033[0m: you can pass size in other");
    printf(" units (default in 512-bytes disk sectors) by:\n");
    printf("K - kilobytes\nM - megabytes\nG - gigabytes\n");
    printf("\n\033[1mNotice\033[0m ");
    printf("that LIFS supports only 2048G (2 Terabytes) disk size!\n");
    printf("\n\n");
}

int format_disk(const char* disk, uint32_t size)
{
    FILE* dd = fopen(disk, "wb");

    if(dd == NULL)
    {
        return 1;
    }

    printf("\n\033[1;31mWARNING!\033[0m You have passed '%s' ", disk);
    printf("as disk for LIFS creation\nin size of ");
    printf("%d sectors (%d bytes).\n", size, size * _LIFS_SECTOR_SIZE_);
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
        printf("You have cancelled LIFS creation.\n\n\n");

        exit(255);
    }

    fseek(dd, size * _LIFS_SECTOR_SIZE_ - 1, 0);
    fputc(0, dd);

    fclose(dd);
}

int main(int argc, const char** argv)
{
    printf("\n\n\033[1m%s\033[0m v. %s", "LIFS creation tool", VERSION);
    printf("\nCopyright (C) eastev <https://github.com/eastev>\n\n");

    if(argc == 1 || argc > 1 && (!strcmp(argv[1], "-h") || 
        !strcmp(argv[1], "--help")))
    {
        print_help();

        return 0;
    }

    if(argc < MIN_ARGS || (argc - 1) % 2 != 0)
    {
        printf("\033[0;31mERROR\033[0m: Too few arguments!\n\n");
        print_help();

        return -1;
    }

    uint32_t size = atoi(argv[argc - 1]);
    uint8_t p_offset = 1;

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--skip"))
        {
            if(atoi(argv[i + 1]) > UINT8_MAX - 1)
            {
                printf("\033[0;31mERROR\033[0m: Reserve size is too large!");
                printf(" (It have to be less than 127 Kilobytes)\n\n\n");

                return -4;
            }

            p_offset = atoi(argv[i + 1]);
        }
    }

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--size"))
        {
            char c = argv[i + 1][0];

            switch(c)
            {
                default: 
                    printf("\033[0;31mERROR\033[0m: ");
                    printf("Wrong size format! ('%c')\n\n\n", c);
                            
                    return -3;
                case 'K': size *= 2; p_offset *= p_offset == 1 ? 
                        1 : 2; 
                    break;
                case 'M': size *= 2 * 1024; p_offset *= p_offset == 1 ? 
                        1 : 2 * 1024;
                    break;
                case 'G': 
                    size *= 2 * 1024 * 1024; p_offset *= p_offset == 1 ? 
                        1 : 2 * 1024 * 1024; 
                    break;
            }
        }
    }

    if(size < _LIFS_MIN_FS_SIZE_)
    {
        printf("\033[0;31mERROR\033[0m: Size is too small!\n\n");
        print_help();

        return -2;
    }
    
    const char* disk = argv[argc - 1 - 1];

    if(format_disk(disk, size))
    {
        printf("\033[0;31mERROR\033[0m: Can't format disk!\n\n\n");

        return -128;
    }

    const char* config = NULL;

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--conf"))
        {
            config = malloc(strlen(argv[i + 1]));
            strcpy(config, argv[i + 1]);

            break;
        }
    }

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--boot"))
        {
            int err;

            if((err = create_mbr(disk, argv[i + 1])) != 0)
            {
                printf("LIFS creation\033[1;31m error\033[0m! Exit...\n\n\n");

                return err;
            }

            break;
        }
    }

    if(partitioning(p_offset, disk, size, config))
    {
        if(config != NULL)
        {
            free(config);
        }   

        printf("\nLIFS creation\033[1;31m error\033[0m! Exit...\n\n\n");

        return 128;
    }

    if(config != NULL)
    {
        free(config);
    }

    printf("\nLifs created\033[0;32m successfully\033[0m!\n\n\n");

    return 0;
}
