#ifndef VMOS_HARDDRIVE_H
#define VMOS_HARDDRIVE_H

#include "architecture.h"

typedef struct
{
    /* data */
} arch_disk_controller;

typedef struct
{
    arch_uint fd;
    arch_uint priority;
    arch_uint address;
    arch_addr text_off;
    arch_uint text_size;
    arch_addr data_off;
    arch_uint data_size;
} arch_file_meta;

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

#endif