#include "scheduler.h"
#include "process.h"
#include "handler.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>

queue ready_queue;
static arch_core** cores;
static vmos_uint core_count;

void init_scheduler(arch_core**, vmos_uint);
void schedule_tasks();
void add_to_queue();
vmos_pcb* remove_from_queue();
vmos_pcb* remove_from_queue_specific(vmos_uint);
void reorganize_queue();



void init_scheduler(arch_core** core_array, vmos_uint size)
{
    cores = core_array;
    core_count = size;

    for (vmos_int i = 0; i < core_count; i++)
    {
        ready_queue.processes[i] = NULL;
    }

    ready_queue.start = 0;
    ready_queue.end = 0;
    ready_queue.current_priority = 0;
    ready_queue.free = core_count;
}

void schedule_tasks()
{
    vmos_int count = 0;
    vmos_pcb** waiting_queue;
    waiting_queue = malloc(sizeof(vmos_pcb*) * core_count);
    for (vmos_int i = 0; i < core_count; i++)
    {
        waiting_queue[i] = NULL;
    }
    while(count < loaded_processes->count)
    {
        if(loaded_processes->list[count] != NULL)
        {
            for (vmos_int i = 0; i < core_count; i++)
            {
                if (waiting_queue[i] == NULL || waiting_queue[i]->pid > loaded_processes->list[count]->pid)
                {
                    if (loaded_processes->list[count]->program_status == loaded)
                    {
                        waiting_queue[i] = loaded_processes->list[count];
                        waiting_queue[i]->program_status = suspended;
                    }
                }
            }
        }
        count++;
    }
    for (vmos_int i = 0; i < core_count; i++)
    {
        add_to_queue(waiting_queue[i]);
    }

    printf("Progress on CPUs: \n");
    for (vmos_int i = 0; i < core_count; i++)
    {
        if (ready_queue.processes[i]->program_status == exit_success || ready_queue.processes[i]->program_status == exit_failure)
        {
            remove_from_queue(i);
        }

        if (cores[i]->pcb_reference == NULL)
        {
            printf("CPU %i is currently idle...\n", i);
        }
        else
        {
            printf("\tCPU %i is currently processing job #%i of priority %i...\n", i, cores[i]->pcb_reference->fd, cores[i]->pcb_reference->pid);
        }
        
        
    }
}

void add_to_queue(vmos_pcb* process)
{
    if (ready_queue.free > 0)
    {
        ready_queue.processes[ready_queue.end] = process;
        ready_queue.current_priority = process->pid;
        if (ready_queue.end == core_count - 1)
        {
            ready_queue.end = 0;
        }
        else
        {
            ready_queue.end++;
        }
        ready_queue.free--;
    }////
}

vmos_pcb* remove_from_queue()
{
    if (ready_queue.free >= core_count)
    {
        return NULL;
    }
    vmos_pcb* process = ready_queue.processes[ready_queue.start];
    if (ready_queue.start == core_count - 1)
    {
        ready_queue.start = 0;
    }
    else
    {
        ready_queue.start++;
    }
    ready_queue.free++;
    return process;
}

vmos_pcb* remove_from_queue_specific(vmos_uint index)
{
    vmos_pcb* process = ready_queue.processes[index];
    ready_queue.processes[index] = NULL;
    reorganize_queue();
    return process;
}

void reorganize_queue()
{
    for (vmos_int i = ready_queue.start; i != ready_queue.end; i++)
    {
        if (ready_queue.processes[i] == NULL)
        {
            for(vmos_int j = i; j != ready_queue.start; j--)
            {
                if (j <= -1)
                {
                    j = core_count - 1;
                }
                if (j != 0)
                {
                    ready_queue.processes[j] = ready_queue.processes[j-1];
                }
                else
                {
                    ready_queue.processes[j] = ready_queue.processes[core_count-1];
                }
            }
        }
    }
}