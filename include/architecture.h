#ifndef NBERAKI_ARCH_H
#define NBERAKI_ARCH_H

#define RAM_SIZE 1024
#define DISK_SIZE 2048

#include "instructions.h"
#include "dma_controller.h"

typedef unsigned int arch_word;
typedef unsigned int arch_addr;

typedef struct arch_core arch_core;
typedef struct arch_dma arch_dma;

extern arch_core* init_core();
extern void cycle(arch_core*);

#endif
