#include "architecture.h"

typedef struct
{
    union status_reg
    {
        arch_word data;
        struct status
        {
            unsigned int power: 1;
            unsigned int full: 1;
            unsigned int empty: 1;
            unsigned int no_conn: 1;

        };
    };
    char storage[DISK_SIZE * 4]; 
} arch_disk;

static arch_disk disk;

void access_disk(arch_dma* disk_dma)
{

}