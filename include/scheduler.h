#ifndef VMOS_SCHEDULER_H
#define VMOS_SCHEDULER_H

#include "process.h"

typedef struct
{
    vmos_pcb** pcb;
    vmos_uint count;
    vmos_uint capacity;
} queue;

init_scheduler(vmos_pcb_list process_list, arch_core* cores);
vmos_pcb* select_process_for_core(arch_uint cpuid);

#endif 