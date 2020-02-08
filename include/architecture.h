#ifndef VMOS_ARCH_H
#define VMOS_ARCH_H

#define RAM_SIZE 1024
#define DISK_SIZE 2048

/* Interrupt Vector Address and Interrupt Offsets*/
#define INTRPT_TABLE 0x0 /* interrupt vector starts at 0x0*/
#define INTRPT_OFF_RESET 0x0 /* Reset the computer */
#define INTRPT_OFF_

#include "instructions.h"
#include "dma_controller.h"

typedef unsigned int arch_word;
typedef unsigned int arch_addr;

typedef struct arch_core arch_core;
typedef struct arch_dma arch_dma;

extern arch_core* init_core();
extern void cycle(arch_core*);

#endif
