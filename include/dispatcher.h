#ifndef VMOS_DISPATCHER_H
#define VMOS_DISPATCHER_H

#include "archE.h"
#include "architecture.h"
#include "process.h"

extern void init_dispatcher(arch_core** core_array, vmos_uint size);
extern void dispatch_cores();

#endif /* dispatcher.h */
