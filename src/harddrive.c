#include "architecture.h"
#include "handler.h"
#include <stdio.h> // to save/load hard drive 
#include <stdbool.h>

typedef struct
{
    /* data */
} arch_disk_controller;

typedef struct
{
    /* data */
} arch_boot_record;

typedef struct
{
    union
    {
        arch_word data;
        struct
        {
            arch_word power:   1;
            arch_word full:    1;
            arch_word empty:   1;
            arch_word no_conn: 1;
            arch_word reading: 1;
            arch_word writing: 1;
        };
    } status_reg;
    char storage[DISK_SIZE * 4]; 
} arch_disk;

void access_disk(const char* path)
{
    FILE* disk_file = NULL;
}

bool write_to_disk(arch_disk* disk, const char* filename)
{
    return false;
}

bool write_to_disk(arch_disk* disk, FILE* file)
{
    return false;
}

