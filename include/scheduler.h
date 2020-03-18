#ifndef VMOS_SCHEDULER_H
#define VMOS_SCHEDULER_H

#include "process.h"

typedef struct
{
    vmos_pcb* pcb;
    vmos_uint count;
    vmos_uint capacity;
} queue;


#endif