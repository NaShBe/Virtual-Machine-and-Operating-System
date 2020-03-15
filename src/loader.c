#include "loader.h"
#include "architecture.h"
<<<<<<< HEAD
#include "harddrive.h"
#include "process.h"

vmos_pcb_list* loaded_processes = NULL;
vmos_pcb_list* unloaded_processes = NULL;

void load_jobs(arch_drive* drive)
=======
#include "process.h"

process_list loaded_processes;

vmos_pcb* load_process(arch_file_meta* file, arch_addr address)
>>>>>>> working on code to adapt new IO
{
    arch_addr current_point;

}

<<<<<<< HEAD
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
=======
}

>>>>>>> working on code to adapt new IO
