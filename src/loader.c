#include "loader.h"
#include "architecture.h"
#include "harddrive.h"
#include "process.h"

vmos_pcb_list* loaded_processes = NULL;
vmos_pcb_list* unloaded_processes = NULL;

void load_jobs(arch_drive* drive)
{
    arch_addr current_point;

}

arch_addr help_find_free_space(vmos_pcb* process)
{
    arch_addr current_point = INTRPT_OFFSET + INTRPT_SIZE;
    vmos_uint accum = 0;
    while (current_point < RAM_SIZE && accum < process->process.size)
    {
        arch_addr overlapped_process;
        for (vmos_uint i = 0; i != overlapped_process; i++)
        {
            
        }
        current_point++;
    }
    

}