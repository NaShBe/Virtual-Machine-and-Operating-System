<<<<<<< HEAD
#include "vmos.h"
=======
#include "harddrive.h"
>>>>>>> working on code to adapt new IO
#include "architecture.h"
#include "handler.h"
#include "parse.h"
#include "harddrive.h"
#include <stddef.h> // for NULL
#include <stdio.h>  // for I/O
#include <stdlib.h> // for malloc()

<<<<<<< HEAD
void init_drive(arch_drive* drive)
{
    drive->file_system.storage = drive->storage;
    drive->file_system.files = NULL;
    drive->device.address = connect_dma(&drive->device);
}

void write_to_disc(arch_drive* drive, job_list* jobs)
{
    arch_uint file_count = jobs->count;
    drive->file_system.files = malloc(sizeof(drive_file*) * file_count);
=======
static arch_file_meta* file_list;
static FILE* input_file;
>>>>>>> working on code to adapt new IO

    for (arch_uint i = 0; i < file_count; i++)
    {
        drive_file* new_file = malloc(sizeof(drive_file));
        new_file->fd = i;
        new_file->priority = jobs->jobs[i]->priority;
        new_file->instr_part = jobs->jobs[i]->instr_count;
        new_file->data_part = jobs->jobs[i]->data_count;
        new_file->instr_buff = jobs->jobs[i]->instructions;
        new_file->data_buff = jobs->jobs[i]->data;
        drive->file_system.files[i] = new_file;
    }
<<<<<<< HEAD
}

drive_file* get_file(arch_drive* drive, arch_uint fd)
=======

    //scan
    if ( 1) // compare scan and see if it is harddrive type
    {

    }
    drive_file* full_file = malloc(sizeof(drive_file));
}

drive_file* get_file_data(arch_file_meta* file)
{
    
}

arch_bool write_to_disc(arch_drive* drive, const char* filename)
>>>>>>> working on code to adapt new IO
{
    return drive->file_system.files[fd];
}

arch_word read_data(arch_uint fd, arch_addr data_addr)
{
    arch_byte* address = (arch_byte*)(main_drive->file_system.files[fd]->data_buff);
    address += data_addr;
    return *(arch_word*)address;
}

void write_data(arch_uint fd, arch_addr data_addr, arch_word data)
{
    arch_byte* address = (arch_byte*)(main_drive->file_system.files[fd]->data_buff);
    address += data_addr;
    *(arch_word*)(address) = data;
}