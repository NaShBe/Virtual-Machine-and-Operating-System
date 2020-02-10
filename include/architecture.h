#ifndef VMOS_ARCH_H
#define VMOS_ARCH_H

#ifndef RAM_SIZE
#define RAM_SIZE 1024
#endif

#ifndef DISK_SIZE
#define DISK_SIZE 2048
#endif

typedef unsigned int arch_word;
typedef unsigned int arch_addr;

typedef struct arch_core arch_core;
typedef struct arch_dma arch_dma;

extern arch_core* init_core();
extern void cycle(arch_core*);

#endif
