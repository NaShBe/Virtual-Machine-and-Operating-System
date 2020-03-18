#include "architecture.h"
#include "handler.h"
#include "parse.h"
#include "harddrive.h"
#include <stddef.h> // for NULL
#include <stdio.h>  // to save/load hard drive
#include <stdlib.h> // for malloc()

void write_to_disc(arch_drive* drive, job_list* jobs)
{
    arch_uint file_count = jobs->count;
    for (arch_uint i = 0; i < file_count; i++)
    {
        drive_file* new_file = malloc(sizeof(drive_file));

        jobs->jobs[i];
    }
    return FALSE;
}

drive_file* get_file(arch_drive* drive, arch_uint fd)
{
    return drive->file_system.files[fd];
}