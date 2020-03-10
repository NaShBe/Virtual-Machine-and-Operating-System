#ifndef VMOS_PROCESS_H
#define VMOS_PROCESS_H

#include "architecture.h"   // needed for arch_registers
#include "vmos.h"           // needed for vmos_uint

typedef enum
{
    running,
    exit_success,
    exit_failure
} status_enum;                      /* used by the OS for scheduling, error handling, and unloading */

typedef struct                      /* every loaded program must have a pcb attached to it */
{
    vmos_uint       pid;            /* unique id attached to job */
    status_enum     program_status; /* whether the program is running, successful, or unsuccessful in execution */
    arch_registers  state_reg;      /* holds the state of the core registers for resuming execution of queued jobs */
} vmos_pcb;

typedef struct
{
    arch_byte* memory;
    vmos_uint  size;
} vmos_process;

#endif