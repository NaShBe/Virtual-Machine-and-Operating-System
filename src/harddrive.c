#include "architecture.h"
#include "handler.h"
#include <stddef.h> // for NULL
#include <stdio.h>  // to save/load hard drive
#include <stdlib.h> // for malloc()

typedef struct
{
    /* data */
} arch_disk_controller;

typedef struct
{
    /* data */
} arch_partition_table;

typedef struct
{
    arch_byte   storage[DISK_SIZE * 4];
    arch_device device;
} arch_drive;

typedef struct
{
    arch_byte*  data;
    arch_uint   size;
} drive_file;


arch_device* access_drive(const char* path)
{
    FILE* disc_file = NULL;
    disc_file = fopen(path, "r");
    if (disc_file == NULL)
    {
        send_error(disc_not_attached);
    }
    drive_file* full_file = malloc(sizeof(drive_file));
}

arch_bool write_to_disc(arch_drive* drive, const char* filename)
{
    return FALSE;
}

arch_bool write_to_disc(arch_drive* drive, FILE* file)
{
    return FALSE;
}

