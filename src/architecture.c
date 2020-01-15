#include "architecture.h"
#include "instructions.h"

#define INTRPT_RESET 0x0
#define INTRPT_UNDEFINED 0x1

#define CORE_PARALLEL FALSE		// This establishes if instruction pipelining happens.
#define CORE_STEPS 3		// This is how many steps are taken to complete one CPU instruction (fetching, decoding, executing)

typedef struct 
{
	arch_word ac;
	const arch_word zr;
	arch_word reg[14];
	arch_addr pc;
	instruction ir;
	arch_word sp;
} arch_registers;

typedef struct
{
	arch_addr reset;
	arch_addr undefined;
	arch_addr software;
} arch_interrupt_table;

typedef struct
{
	arch_word oprand1;
	arch_word oprand2;
} arch_alu;

typedef struct
{

} arch_dma;


typedef struct
{
	arch_registers core_regs;
	arch_alu alu;
	void (*pipeline[CORE_STEPS])(arch_core*);
} arch_core;

static volatile char ram[RAM_SIZE * 4] __attribute__((section("nberaki_ram")));

arch_core* init_core()
{
	static arch_core core = {.core_regs = {0}, .pipeline = {fetch, decode, no_op}};
	return &core;
}

void cycle (arch_core* core)
{
	for (unsigned int i = 0; i < CORE_STEPS; i++)
	{
		core->pipeline[i](core);
	}
}

static inline void fetch(arch_core* core)
{
	core->core_regs.ir.instr_data = core->core_regs.pc;
	core->core_regs.pc++;
}

static void decode(arch_core* core)
{
	switch GET_FORMAT(core->core_regs.ir.instr_data)
	{
		case FORMAT_AIF:
			if (0xFFFF & core->core_regs.ir.instr_data != 0) return; //TODO
			switch (GET_OPCODE(core->core_regs.ir.instr_data))
			{
				case MOV:
					break;
				case ADD:
					break;
				case SUB:
					break;
				case MUL:
					break;
				case DIV:
					break;
				case AND:
					break;
				case OR:
					break;
				case SLT:
					break;
				default:
					return;
			}
			break;
		case FORMAT_CBIF:
			switch (GET_OPCODE(core->core_regs.ir.instr_data))
			{
				case 0:			//TODO
					break;
			}
			break;
		case FORMAT_UJF:
			switch (GET_OPCODE(core->core_regs.ir.instr_data))
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
			switch (GET_OPCODE(core->core_regs.ir.instr_data))
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