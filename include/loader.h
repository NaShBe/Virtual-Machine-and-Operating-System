#ifndef VMOS_LOADER_H
#define VMOS_LOADER_H

#include "vmos.h"

typedef struct
{
    vmos_pcb* processes;
    vmos_uint count;
    vmos_uint capacity;
} process_list;

extern process_list loaded_processes;

vmos_pcb* load_process();

#endif