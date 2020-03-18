#ifndef VMOS_DISPATCHER_C
#define VMOS_DISPATCHER_C

#include "dispatcher.h"
#include "process.h"
#include "architecture.h"
#include "scheduler.h"

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

void check_process_status(arch_core* core)
{
    

}








#endif