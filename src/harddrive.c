#include "architecture.h"
#include <stdio.h> // to save/load hard drive 

typedef struct
{
    /* data */
} arch_disk_controller;


typedef struct
{
    /* data */
} arch_boot_record;

typedef struct
{
    union
    {
        arch_word data;
        struct
        {
            unsigned int power: 1;
            unsigned int full: 1;
            unsigned int empty: 1;
            unsigned int no_conn: 1;
        };
    } status_reg;
    char storage[DISK_SIZE * 4] __attribute__((section("nberaki_disk"))); 
} arch_disk;

static arch_disk disk;

void access_disk(arch_dma* disk_dma)
{
    while(disk.status_reg.power)
    {
        if (disk_dma == NULL)
        {

        }
        else
        {
            
        }
        
    }
}