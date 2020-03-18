#ifndef VMOS_LOADER_H
#define VMOS_LOADER_H

<<<<<<< HEAD
#include "process.h"

extern vmos_pcb_list* loaded_processes;
extern vmos_pcb_list* unloaded_processes;
=======
#include "vmos.h"

typedef struct
{
    vmos_pcb* processes;
    vmos_uint count;
    vmos_uint capacity;
} process_list;

extern process_list loaded_processes;

<<<<<<< HEAD
void load_process();
>>>>>>> working on code to adapt new IO
=======
vmos_pcb* load_process();
>>>>>>> scheduler

#endif