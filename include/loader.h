#ifndef VMOS_LOADER_H
#define VMOS_LOADER_H

#include "process.h"

extern vmos_pcb_list* loaded_processes;
extern vmos_pcb_list* unloaded_processes;

extern void get_jobs(arch_drive*);
extern void output_loader(vmos_char*);
#endif