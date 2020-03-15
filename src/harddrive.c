#include "harddrive.h"
#include "architecture.h"
#include "handler.h"
#include "parse.h"
#include "harddrive.h"
#include <stddef.h> // for NULL
#include <stdio.h>  // for I/O
#include <stdlib.h> // for malloc()

static arch_file_meta* file_list;
static FILE* input_file;

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
{
    return FALSE;
}

drive_file* get_file(arch_drive* drive, arch_uint fd)
{
    return drive->file_system.files[fd];
}