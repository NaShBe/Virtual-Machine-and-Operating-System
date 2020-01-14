#ifndef NBERAKI_ARCH_H
#define NBERAKI_ARCH_H

#define RAM_SIZE 1024U
#define DISK_SIZE 2048U

#include "instructions.h"

typedef unsigned int arch_word;
typedef arch_word arch_addr;

typedef struct 
{
	arch_word ac;
	const arch_word zr;
	arch_word reg[14];
	arch_addr pc;
	instruction_t ir;
	arch_word sp;
} registers;

extern registers cpu_regs __attribute__((section("nberaki_cpu_regs")));

#endif
