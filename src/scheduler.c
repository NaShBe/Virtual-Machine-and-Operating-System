#include "scheduler.h"
#include "process.h"
#include "handler.h"
#include "loader.h"
#include <unistd.h>
#include <stdlib.h>

static queue* ready_queue;
static queue* wait_queue;

queue* init_queues();
add_load_to_ready_queue(process_list process_list);
add_process_to_ready_queue(vmos_pcb* process);
watch_queue();
wait_to_ready(vmos_uint pid);
reorder_ready(queue* ready_queue);
void swap(vmos_pcb *xp, vmos_pcb *yp);
void bubbleSort(vmos_pcb* arr[], arch_int n);
wait_to_ready(vmos_uint pid);
send_to_wait(queue* wait_queue, vmos_pcb* process);
void help_add_to_queue(queue* process_queue, vmos_pcb* pcb);

queue* init_queues() 
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

add_load_to_ready_queue(process_list process_list)
{
    init_queues();
    for(vmos_uint i = 0; i < process_list.count + 1; i++) 
    {
        process_list.processes[i].program_status = ready;
        help_add_to_queue(&ready_queue, &process_list.processes[i]);
    }
    reorder_ready(&ready_queue);
}

add_process_to_ready_queue(vmos_pcb* process)
{
    process->program_status = ready;
    help_add_to_queue(ready_queue, process);
    reorder_ready(ready_queue);
}

watch_queue() 
{
    while(ready_queue->count != 0)
    {
        reorder_ready(&ready_queue);
    }
}

/* Uses 'bubble sort' */
reorder_ready(queue* ready_queue) 
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
    wait_queue = realloc(ready_queue->pcb, sizeof(process) + sizeof(wait_queue));
    if (ready_queue->pcb == NULL) 
    {
        send_error(bad_malloc);
    }
    wait_queue->count =+ 1;
    wait_queue->capacity = sizeof(wait_queue);
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
    process_queue->pcb[process_queue->count] = pcb;
}