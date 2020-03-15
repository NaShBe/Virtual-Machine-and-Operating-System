#ifndef VMOS_HARDDRIVE_H
#define VMOS_HARDDRIVE_H

#include "architecture.h"
<<<<<<< HEAD
#include "instructions.h"
#include "parse.h"

typedef struct
{
    arch_uint   fd;
    arch_uint   priority;
    arch_uint   instr_part;
    arch_instr* instr_buff;
    arch_uint   data_part;
    arch_word*  data_buff;
} drive_file;
=======
>>>>>>> working on code to adapt new IO

typedef struct
{
    /* data */
} arch_disk_controller;

typedef struct
{
<<<<<<< HEAD
    arch_byte*      storage;
    arch_uint       file_count;
    drive_file**    files;
} arch_files;
=======
    arch_uint fd;
    arch_uint priority;
    arch_uint address;
    arch_addr text_off;
    arch_uint text_size;
    arch_addr data_off;
    arch_uint data_size;
} arch_file_meta;
>>>>>>> working on code to adapt new IO

typedef struct
{
    arch_byte   storage[DISK_SIZE * 4];
<<<<<<< HEAD
    arch_files  file_system;
    arch_device device;
} arch_drive;

extern arch_drive*    main_drive;

extern void         init_drive(arch_drive* drive);
extern void         write_to_disc(arch_drive* drive, job_list* jobs);
extern drive_file*  get_file(arch_drive* drive, arch_uint fd);
arch_word           read_data(arch_uint, arch_addr);
void                write_data(arch_uint, arch_addr, arch_word);
=======
    arch_device device;
} arch_drive;

typedef struct
{
    arch_byte*  data;
    arch_uint   size;
} drive_file;
>>>>>>> working on code to adapt new IO

#endif