#ifndef VMOS_SCHEDULER_H
#define VMOS_SCHEDULER_H

#include "process.h"
#include "architecture.h"

typedef struct
{
    vmos_pcb* processes[CORE_COUNT];
    vmos_uint start;
    vmos_uint end;
    vmos_uint free;
    vmos_int current_priority;
} queue;

extern queue ready_queue;

extern void         init_scheduler(arch_core**, vmos_uint);
extern void         schedule_tasks();
extern vmos_pcb*    remove_from_queue();

#endif /* scheduler.h */
