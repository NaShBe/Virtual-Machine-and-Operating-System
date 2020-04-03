#include "dispatcher.h"
#include "process.h"
#include "architecture.h"
#include "scheduler.h"
#include "handler.h"
#include "loader.h"

static arch_core**  cores;
static vmos_uint    core_count;

void init_dispatcher(arch_core** core_array, vmos_uint size);
void dispatch_cores();
void stage_ready_process(arch_core*, vmos_pcb*);
void unstage_process(arch_core*, vmos_pcb*);


void init_dispatcher(arch_core** core_array, vmos_uint size)
{
    cores = core_array;
    core_count = size;
}

void dispatch_cores()
{
    for (vmos_int i = 0; i < core_count; i++)
    {
        if (cores[i]->pcb_reference == NULL && help_get_instruction(cores[i]) == HTL)
        {
            cores[i]->pcb_reference == remove_from_queue();
            if (cores[i]->pcb_reference == NULL)
            {
                continue;
            }
            cores[i]->regs.pc = cores[i]->pcb_reference->process.memory;
            cores[i]->pcb_reference->cpuid = cores[i]->id;
        }
        else if (help_get_instruction(cores[i]) == HTL)
        {
            cores[i]->pcb_reference->program_status = exit_success;
            cores[i]->pcb_reference = NULL;
        }
    }
}

/* dispatcher.c */ 