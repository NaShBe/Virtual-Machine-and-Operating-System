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
    
}

void stage_ready_process(arch_core* core, vmos_pcb* pcb)
{
    core->regs = pcb->state_reg;
    core->pcb_reference = pcb;
    pcb->program_status = running;
    //set cpu to run the process
}

void unstage_process(arch_core* core, vmos_pcb* pcb)
{
    pcb->state_reg = core->regs;
    pcb->program_status = exit_success;
    free_jobs();
}
/* dispatcher.c */ 