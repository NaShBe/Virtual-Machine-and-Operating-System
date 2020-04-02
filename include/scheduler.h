#ifndef VMOS_SCHEDULER_H
#define VMOS_SCHEDULER_H

#include "process.h"
#include "architecture.h"

typedef struct
{
    vmos_pcb* processes[CORE_COUNT];
    vmos_uint count;
    vmos_int current_priority;
} queue;

extern void init_scheduler(arch_core**, vmos_uint);
extern void schedule_tasks();

#endif 