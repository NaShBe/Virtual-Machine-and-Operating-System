#ifndef VMOS_HARDDRIVE_H
#define VMOS_HARDDRIVE_H

#include "architecture.h"
#include "instructions.h"

typedef struct
{
    /* data */
} arch_disk_controller;

typedef struct
{
    arch_byte*      storage;
    arch_uint       file_count;
    arch_uint       file_capacity;
    drive_file**    files;
} arch_files;

typedef struct
{
    arch_byte   storage[DISK_SIZE * 4];
    arch_files  file_system;
    arch_device device;
} arch_drive;

typedef struct
{
    arch_uint   fd;
    arch_uint   priority;
    arch_uint   instr_part;
    arch_instr* instr_buff;
    arch_uint   data_part;
    arch_word*  data_buff;
} drive_file;

#endif