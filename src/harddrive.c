#include "harddrive.h"
#include "architecture.h"
#include "handler.h"
#include <stddef.h> // for NULL
#include <stdio.h>  // for I/O
#include <stdlib.h> // for malloc()

static arch_file_meta* file_list;
static FILE* input_file;

arch_device* access_drive(const char* path)
{
    FILE* disc_file = NULL;
    disc_file = fopen(path, "r");
    if (disc_file == NULL)
    {
        send_error(disc_not_attached);
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

arch_bool write_to_disc(arch_drive* drive, FILE* file)
{
    return FALSE;
}

