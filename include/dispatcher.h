#ifndef VMOS_DISPATCHER_H
#define VMON_DISPATCHER_H


#include "archE.h"
#include "architecture.h"
#include "process.h"

/**
 * Swaps the current process in the cpu to a ready process from scheduler
 */
extern void swap_process(arch_core*);

#endif /* dispatcher.h */
