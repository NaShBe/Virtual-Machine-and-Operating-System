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
add_process_to_ready_queue(vmos_pcb process);
watch_queue();
wait_to_ready(vmos_uint pid);
reorder_ready(queue* ready_queue);
void bubbleSort(vmos_pcb* arr[], arch_int n);
wait_to_ready(vmos_uint pid);
send_to_wait(queue* wait_queue, vmos_pcb process);

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
    ready_queue = realloc(ready_queue->pcb, sizeof(process_list));
    ready_queue->count = process_list.count;
    ready_queue->capacity = sizeof(ready_queue);
    reorder_ready(ready_queue);
}

add_process_to_ready_queue(vmos_pcb process)
{
    ready_queue = realloc(ready_queue->pcb, sizeof(process) + sizeof(ready_queue->pcb));

    ready_queue->count =+ 1;
    ready_queue->capacity = sizeof(ready_queue);
    arch_int index = ready_queue->count + 1;

    ready_queue->pcb[index] = process;
    ready_queue->pcb[index].program_status = ready;
    reorder_ready(ready_queue);
}

watch_queue() 
{
    while(ready_queue->count != 0)
    {
        reorder_ready(ready_queue);
    }
}

/* Uses 'bubble sort' */
reorder_ready(queue* ready_queue) 
{
    arch_int n = sizeof(ready_queue->pcb) / sizeof(ready_queue->pcb[0]);
    bubbleSort(ready_queue->pcb, n);
}

void bubbleSort(vmos_pcb* arr[], arch_int n) 
{ 
   int i, j; 
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
        if (pid == wait_queue->pcb[i].pid) {
            vmos_pcb process = wait_queue->pcb[i];
            add_process_to_ready_queue(process);

            vmos_pcb swap_process = wait_queue->pcb[wait_queue->count + 1];
            wait_queue->pcb[i] = swap_process;

            realloc(wait_queue->pcb, sizeof(wait_queue->pcb) - sizeof(swap_process));
            wait_queue->count =- 1;
            wait_queue->capacity = sizeof(wait_queue);
            return;
        }
    }
}

send_to_wait(queue* wait_queue, vmos_pcb process) 
{
    wait_queue = realloc(ready_queue->pcb, sizeof(process) + sizeOf(wait_queue));
    wait_queue->count =+ 1;
    wait_queue->capacity = sizeof(wait_queue);
}