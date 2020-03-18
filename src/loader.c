#include "loader.h"
#include "architecture.h"
#include "harddrive.h"
#include "process.h"
#include "handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

vmos_pcb_list* loaded_processes = NULL;
vmos_pcb_list* unloaded_processes = NULL;

void get_jobs(arch_drive* drive)
{
    unloaded_processes = malloc(sizeof(vmos_pcb_list));
    unloaded_processes->list = malloc(sizeof(vmos_pcb) * drive->file_system.file_count);

    for (vmos_uint i = 0; i < drive->file_system.file_count; i++)
    {
        vmos_pcb* process = malloc(sizeof(vmos_pcb));
        if (process == NULL)
        {
            send_error(bad_malloc);
        }
        unloaded_processes->list[i] = process;
    }
}

arch_addr help_find_free_space(vmos_pcb* process)
{
    arch_addr current_point = INTRPT_OFFSET + INTRPT_SIZE;
    arch_addr possible_space = current_point;
    vmos_uint accum = 0;
    while (current_point < RAM_SIZE && accum < process->process.size)
    {
        arch_addr overlapped_process;
        for (vmos_uint i = 0; i < loaded_processes->count; i++)
        {
            overlapped_process = help_get_arch_addr(loaded_processes->list[i]->process.memory);
            if (current_point == overlapped_process)
            {
                current_point = overlapped_process + loaded_processes->list[i]->process.size;
                possible_space = current_point;
                accum = 0;
            }
        }
        current_point++;
        accum++;
    }
    if (accum >= process->process.size)
    {
        return possible_space;
    }
    else
    {
        return 0;
    }
}

void output_loader(vmos_char* file)
{
    static vmos_bool has_read = FALSE;
    FILE* output_file = NULL;
    if( access( file, F_OK ) != -1 ) {
        // file exists
        output_file = fopen(file, "w+");
    }
    else
    {
        // file doesn't exist
        output_file = fopen(file, "w");
    }

    if (has_read == FALSE)
    {
        fprintf(output_file, "\n\n\nLoader has read the following processes:\n");
        for (vmos_uint i; i < unloaded_processes->count; i++)
        {
            fprintf(output_file, "\t- Process %i:\tinstruction count: %i\tPriority: %i\n", i+1, unloaded_processes->list[i]->process.size, unloaded_processes->list[i]->pid);
            fprintf(output_file, "\t\tRead from file %i\n", unloaded_processes->list[i]->fd);
        }
        has_read = TRUE;
    }
    
    fprintf(output_file, "\n\nThe current processes are loaded into memory:");
    for (vmos_uint i; i < loaded_processes->count; i++)
    {
        fprintf(output_file, "\t- Process %i:\tinstruction count: %i\tPriority: %i\n", i+1, loaded_processes->list[i]->process.size, loaded_processes->list[i]->pid);
        fprintf(output_file, "\t\tRead from file %i\n", loaded_processes->list[i]->fd);
        switch (loaded_processes->list[i]->program_status)
        {
            case loaded:
                fprintf(output_file, "\t\t\tThis process is in the waiting queue.");
            case suspended:
                fprintf(output_file, "\t\t\tThis program is currently waiting.");
            case running:
                fprintf(output_file, "\t\t\tThis process is currently running.");
            case exit_success:
                fprintf(output_file, "\t\t\tThis process executed successfully.");
            case exit_failure:
                fprintf(output_file, "\t\t\tThis process executed unsuccessfully.");
            default:
                fprintf(output_file, "\t\t\tNo program status set for this process...");
        }
    }
}