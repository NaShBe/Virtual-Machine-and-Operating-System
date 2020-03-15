#include "scheduler.h"
#include "process.h"

static vmos_pcb* wait_queue;
static vmos_pcb* ready_queue;

