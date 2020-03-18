#ifndef VMOS_DISPATCHER_C
#define VMOS_DISPATCHER_C

#include "dispatcher.h"
#include "process.h"
#include "vmos.h"
#include "architecture.h"

vmos_pcb* outbound_process;

/** on detect process finished halt or context switch **\
 * set outbound process pcb to current cpu information
 * set outbound process pcb status to exit_successful
 * get inbound process from scheduler
 * set curr cpu to inbound process
 * instruct cpu to begin execution
 */

/** on detect context switch
 * 
 */


void set_process(arch_core* core, vmos_pcb* scheduled_process)
{
    core->regs = (scheduled_process->state_reg);
    //set cpu to begin execution
}

void store_process_info(arch_core* core, vmos_pcb* outbound_process)
{
    if(outbound_process->program_status == 0) //interrupted process
    {
        outbound_process->program_status = 1;
        outbound_process->state_reg = core->regs;
        //store process on memory
        return;
    }

    else if(outbound_process->program_status == 2) //completed process
    {
        //dont store anything
        //signal to loader that memory is free
        return;
    }
}


#endif