#include "scheduler.h"
#include "process.h"
#include "handler.h"
#include "loader.h"
#include <unistd.h>
#include <stdlib.h>

static queue ready_queue;
static arch_core** cores;
static vmos_uint core_count;

void init_scheduler(arch_core** core_array, vmos_uint size)
{
    cores = core_array;
    core_count = size;

    for (vmos_int i = 0; i < core_count; i++)
    {
        ready_queue.processes[i] = NULL;
    }
    ready_queue.current_priority = 0;
    ready_queue.count = 0;
}

void schedule_tasks()
{

}