#include "architecture.h"
#include "instructions.h"

#define INTRPT_RESET 0x0
#define INTRPT_UNDEFINED 0x1

#define CORE_PARALLEL FALSE		// This establishes if instruction pipelining happens.
#define CORE_STEPS 3		// This is how many steps are taken to complete one CPU instruction (fetching, decoding, executing)
// NOTE: execution step must always be the last step.

typedef struct 
{
	arch_word ac;
	const arch_word zr;
	arch_word gen_reg[14];
	arch_addr pc;
	instruction ir;
	arch_word sp;
} arch_registers;

typedef struct
{
	arch_addr reset;
	arch_addr undefined;
	arch_addr dma;
	arch_addr ;
} arch_interrupt_table;

typedef struct
{
	arch_word oprand1;
	arch_word oprand2;
	arch_word instr;
	arch_addr output;
} arch_alu;

typedef struct
{
		
} arch_dma;


typedef struct
{
	arch_registers regs;
	arch_alu alu;
	void (*pipeline[CORE_STEPS])(arch_core*);
} arch_core;

static volatile char ram[RAM_SIZE * 4] __attribute__((section("nberaki_ram")));

arch_core* init_core()
{
	static arch_core core = {.regs = {0}, .pipeline = {fetch, decode, no_op}};
	return &core;
}

void cycle(arch_core* core)
{
	for (unsigned int i = 0; i < CORE_STEPS; i++)
	{
		core->pipeline[i](core);
	}
}

static inline void fetch(arch_core* core)
{
	core->regs.ir.instr_data = core->regs.pc;
	core->regs.pc++;
}

static void decode(arch_core* core)
{
	switch (core->regs.ir.format)
	{
		case FORMAT_AIF:
			if (((arith_data)core->regs.ir.data).zr != 0)
			{
				return; //TODO, perform undefined instruction interrupt;
			}
			switch (core->regs.ir.opcode)
			{
				case SLT: case MOV ... OR:
					core->alu.instr = core->regs.ir.opcode;
					core->alu.oprand1 = ((arith_data)core->regs.ir.data).src1;
					core->alu.oprand2 = ((arith_data)core->regs.ir.data).src2;
					core->pipeline[CORE_STEPS - 1] = arithm;
					return;
				default:
					return; //TODO, perform undefined instruction interrupt;
			}
			break;
		case FORMAT_CBIF:
			switch (core->regs.ir.opcode)
			{
				case 0:			//TODO
					break;
			}
			break;
		case FORMAT_UJF:
			switch (core->regs.ir.opcode)
			{
				case HTL:
					break;
				case JMP:
					break;
				default:
					return;		//TODO
			}
			break;
		case FORMAT_IOIF:
			switch (core->regs.ir.opcode)
			{
				case RD:
					break;
				case WR:
					break;
				default:
					return;		//TODO
			}
			break;
		default:
			return;		//TODO
	}
}

static void inline no_op(arch_core* core)
{
	return;
}

static void read(arch_core* core)
{

}

static void write(arch_core* core)
{

}

static void store(arch_core* core)
{

}

static void load(arch_core* core)
{

}

static void arithm(arch_core* core)
{
	switch(core->alu.instr)
	{
		case MOV:
			break;	//TODO
		case ADD:
			break;
		case SUB:
			break;
		case MUL:
			break;
		case DIV:
			break;
	}
}