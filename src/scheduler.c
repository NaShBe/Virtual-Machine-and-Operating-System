#include "scheduler.h"
#include "process.h"
<<<<<<< HEAD
#include "handler.h"
#include "loader.h"
#include <unistd.h>
#include <stdlib.h>

static queue* ready_queue;
static queue* wait_queue;
static const arch_bool FIFO; 

queue* create_queues();
init_scheduler(vmos_pcb_list process_list, arch_core* cores);
add_process_to_ready_queue(vmos_pcb* process);
wait_to_ready(vmos_uint pid);
reorder_ready();
send_to_wait(queue* wait_queue, vmos_pcb* process);
void swap(vmos_pcb *xp, vmos_pcb *yp);
void bubbleSort(vmos_pcb* arr[], arch_int n);
wait_to_ready(vmos_uint pid);
send_to_wait(queue* wait_queue, vmos_pcb* process);
vmos_pcb* select_process_for_core(arch_uint cpuid);
pop_from_ready();
void help_add_to_queue(queue* process_queue, vmos_pcb* pcb);

queue* create_queues() 
{
    ready_queue = malloc(sizeof(queue));
    wait_queue = malloc(sizeof(queue));

    if (ready_queue == NULL || wait_queue == NULL) 
    {
        send_error(bad_malloc);
    } 
    ready_queue->capacity = sizeof(ready_queue);
    wait_queue->capacity = sizeof(wait_queue);
    ready_queue->count = 0;
    wait_queue->count = 0;
}

init_scheduler(vmos_pcb_list process_list, arch_core* cores)
{
    create_queues();
    vmos_int core_number = sizeof(cores)/sizeof(cores[0]);

    for(vmos_uint i = 0; i < process_list.count + 1; i++) 
    {
        process_list.list[i]->program_status = loaded;
        help_add_to_queue(&ready_queue, process_list.list[i]);
    }
    if (FIFO == 'false') 
    {
        reorder_ready(&ready_queue);
    }
    if (core_number > 1) 
    {
        for (vmos_int i = 0; i < core_number; i++) 
        {
            ready_queue->pcb[i]->cpuid = cores[i].id;
        }
    }
}

add_process_to_ready_queue(vmos_pcb* process)
{
    process->program_status = loaded;
    help_add_to_queue(ready_queue, process);
    if (FIFO == 'false') 
    {
        reorder_ready();
    }
}

/* Uses 'bubble sort' */
reorder_ready() 
{
    arch_int n = ready_queue->count;
    bubbleSort(&ready_queue->pcb, n);
}

void swap(vmos_pcb *xp, vmos_pcb *yp) 
{ 
    vmos_pcb *temp = xp; 
    xp = yp; 
    yp = temp; 
} 

void bubbleSort(vmos_pcb *arr[], arch_int n) 
{ 
   vmos_uint i, j; 
   for (i = 0; i < n-1; i++)
   {  
       // Last i elements are already in place    
       for (j = 0; j < n-i-1; j++)
       {
           if (arr[j]->pid > arr[j+1]->pid)
           {
              swap(&arr[j], &arr[j+1]);
           }
       }
   }
}

wait_to_ready(vmos_uint pid)
{
    for (arch_int i = 0; i < wait_queue->count + 1; i++)
    {
        if (pid == wait_queue->pcb[i]->pid) {
            vmos_pcb process = *wait_queue->pcb[i];
            add_process_to_ready_queue(&process);

            vmos_pcb *swap_process = &(wait_queue->pcb[wait_queue->count + 1]);
            wait_queue = swap_process;

            wait_queue->count =- 1;
            wait_queue->capacity = sizeof(wait_queue);
            return;
        }
    }
}

send_to_wait(queue* wait_queue, vmos_pcb* process) 
{
    help_add_to_queue(wait_queue, process);
}

vmos_pcb* select_process_for_core(arch_uint cpuid) 
{
    if(FIFO == 'false') 
    {
        reorder_ready();
    }
    (ready_queue->pcb[0]->cpuid) = cpuid;
    pop_from_ready();
    return ready_queue->pcb[0];
}

pop_from_ready() 
{
    for (vmos_int i = 0; i < ready_queue->count; i++) 
    {
        ready_queue->pcb[i] = ready_queue->pcb[i+1];
        ready_queue->count--;
    }
}

void help_add_to_queue(queue* process_queue, vmos_pcb* pcb)
{
    if (process_queue->count >= process_queue->capacity)
    {
        process_queue->capacity += sizeof(queue);
        process_queue->pcb = realloc(process_queue->pcb, process_queue->capacity);
        if (process_queue->pcb == NULL)
        {
            send_error(bad_malloc);
        }
    }
    process_queue->count = process_queue->count++;
    process_queue->pcb[process_queue->count + 1] = pcb; 
} 
=======

static vmos_pcb* wait_queue;
static vmos_pcb* ready_queue;

>>>>>>> working on code to adapt new IO
