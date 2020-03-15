#ifndef VMOS_LOADER_H
#define VMOS_LOADER_H

<<<<<<< HEAD
#include "process.h"
#include "harddrive.h"

extern vmos_pcb_list* loaded_processes;
extern vmos_pcb_list* unloaded_processes;

extern void free_jobs();
extern void load_jobs();
extern void get_jobs(arch_drive*);
extern void output_loader(vmos_char*);
=======
#include "vmos.h"

typedef struct
{
    vmos_pcb* processes;
    vmos_uint count;
    vmos_uint capacity;
} process_list;

extern process_list loaded_processes;

void load_process();

>>>>>>> working on code to adapt new IO
#endif