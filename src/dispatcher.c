#ifndef VMOS_DISPATCHER_C
#define VMOS_DISPATCHER_C

#include "dispatcher.h"
#include "process.h"
#include "architecture.h"
#include "scheduler.h"
#include "handler.h"
#include "loader.h"

vmos_pcb* cpu_process;
vmos_pcb* incoming_process;

void swap_process(arch_core* core) 
{
    cpu_process = core->pcb_reference;
    if(cpu_process == NULL)
    {
        send_error(undefined);
    }
    incoming_process = select_process_for_core(core->id); 
    if(incoming_process == NULL)
    {
        core->regs.ir = cpu_process->rtend;
        return;
    }
    unstage_process(core, cpu_process);
    stage_ready_process(core, incoming_process);
}

void init_dispatch(arch_core* core) 
{
    incoming_process = select_process_for_core(core->id); 
    stage_ready_process(core, incoming_process);
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
#endif /* dispatcher.c */ 