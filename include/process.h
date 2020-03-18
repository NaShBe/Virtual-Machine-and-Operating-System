#ifndef VMOS_PROCESS_H
#define VMOS_PROCESS_H

#include "architecture.h"   // needed for arch_registers
#include "harddrive.h"
#include "vmos.h"           // needed for vmos_uint


typedef enum
{
    running,
    suspended,
    exit_success,
    exit_failure,
    loaded,
    suspended
} status_enum;                      /* used by the OS for scheduling, error handling, and unloading */

typedef struct
{
    arch_addr memory;
    vmos_uint  size;
} vmos_process;

typedef struct                      /* every loaded program must have a pcb attached to it */
{
    vmos_uint           pid;            /* unique id attached to job */
    vmos_uint           fd;             /* which file this is (for data input) */
    status_enum         program_status; /* whether the program is running, successful, or unsuccessful in execution */
    arch_registers      state_reg;      /* holds the state of the core registers for resuming execution of queued jobs */
    arch_uint           cpuid;          /* the id of the cpu processing this job */
    vmos_process        process;        /* the actual process in memory */
    const arch_instr    rtend;          /* this is where the cpu will hang until it is redirected by the dispatcher */
} vmos_pcb;

typedef struct
{
    vmos_uint count;
    vmos_uint capacity;
    vmos_pcb** list;
} vmos_pcb_list;



#endif