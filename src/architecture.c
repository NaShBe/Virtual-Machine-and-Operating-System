/*	=========================================
 * architecture.c - implementation of the main VM architecture
 * 
 * this header should be used for documentation, but as for now it will hold temporary rules for the code:
 * 		1) any VM system errors or C program errors should abort the program
 * 		2) any errors created directly by VM user/programmer or VM programs should invoke VM system error handling (e.g. interrupts)
 * 
 * 
 * 
 * 
 * 
 * 
 *	========================================= 
 */


#include <stddef.h>		// for NULL
#include "architecture.h"
#include "instructions.h"
#include "handler.h"

#define INTRPT_RESET 0x0
#define INTRPT_UNDEFINED 0x1

#define CORE_PARALLEL FALSE		// This establishes if instruction pipelining happens.
#define CORE_STEPS 3		// This is how many steps are taken to complete one CPU instruction (fetching, decoding, executing)
// NOTE: execution step must always be the last step.

typedef struct 
{
	arch_word ac;				/* accumulator */
	const arch_word zr;			/* zero register */
	arch_word gen_reg[10];		/* general-purpose registers */
	arch_addr pc;				/* program counter */
	instruction ir;				/* instruction register */
	arch_addr sp;				/* stack pointer */
	arch_addr bp;				/* base pointer */
} arch_registers;		/* instructions provide access to 16 registers */

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
	arch_word* output;
} arch_alu;

struct arch_core
{
	arch_registers regs;
	arch_alu alu;
	void (*pipeline[CORE_STEPS])(arch_core*);
};

struct arch_dma
{
    arch_addr channel;
	arch_word count;
	arch_word intrpt;

};

static volatile char ram[RAM_SIZE * 4] __attribute__((section("vmos_ram")));

// externally available functions
arch_core* init_core();
void cycle(arch_core*);

// interally available functions
static inline void fetch(arch_core*);
static void decode(arch_core*);
static void inline no_op(arch_core*);
static void read(arch_core*);
static void write(arch_core*);
static void store(arch_core*);
static void load(arch_core*);
static void arithm(arch_core*);
static void jump(arch_core*);

// helper functions
static void help_write_to_mem(char*, unsigned int, arch_addr);
static arch_word* help_get_reg(arch_core*, unsigned int);

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
	core->regs.ir.int_rep = *((arch_word*)&ram[core->regs.pc]);
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
					core->alu.output = help_get_reg(core, ((arith_data)core->regs.ir.data).dest);
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
	unsigned int eff_addr;
	if (((cond_imm_data)core->regs.ir.data).dreg == 0) eff_addr = ((cond_imm_data)core->regs.ir.data).addr;
	else eff_addr = ((cond_imm_data)core->regs.ir.data).dreg;
	unsigned int eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	arch_word* reg = help_get_reg(core, eff_reg);
	eff_addr = *help_get_reg(core, eff_addr);
	if (reg != NULL) help_write_to_mem(reg, sizeof(arch_word), eff_addr);
	else send_error(reg_invalid);
}

static void load(arch_core* core)
{
	unsigned int eff_addr;
	if (((cond_imm_data)core->regs.ir.data).dreg == 0) eff_addr = ((cond_imm_data)core->regs.ir.data).addr;
	else eff_addr = ((cond_imm_data)core->regs.ir.data).dreg;
	arch_word* temp = help_get_reg(core, eff_addr);
	eff_addr = *temp;
	unsigned int eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	if (eff_addr + 4 < RAM_SIZE*4-1) *help_get_reg(core, eff_reg) = ram[eff_addr];
	else send_error(ram_outbounds);
}

static void arithm(arch_core* core)
{
	switch(core->alu.instr)
	{
		case MOV:
			*core->alu.output = core->alu.oprand2; //TODO: OR core->alu.operand1, read documentation
			break;	//TODO
		case ADD:
			*core->alu.output = core->alu.oprand1 + core->alu.oprand2;
			break;
		case SUB:
			*core->alu.output = core->alu.oprand1 - core->alu.oprand2;
			break;
		case MUL:
			*core->alu.output = core->alu.oprand1 * core->alu.oprand2;
			break;
		case DIV:
			if (core->alu.oprand2 !=0) *core->alu.output = core->alu.oprand1 / core->alu.oprand2;
			else; //TODO: divide by zero interrupt
			break;
	}
}

static void jump(arch_core* core)
{
	core->regs.pc = core->regs.ir.data;
}

/*		HELPER FUNCTIONS		*/

static void help_write_to_mem(char* data, unsigned int size, arch_addr addr)
{
	char* offset = &(ram[addr]);
	unsigned int count = 0;
	while (offset <= &(ram[(RAM_SIZE * 4)-1]) && size < count)
	{
		*offset = data[count];
		count++;
		offset++;
	}
	if (offset >= &(ram[(RAM_SIZE * 4)-1])) send_error(ram_outbounds);
}

static arch_word* help_get_reg(arch_core* core, unsigned int eff_reg)
{
	switch(eff_reg)
	{
		case 0:
			return &core->regs.ac;
		case 1:
			return &core->regs.zr;
		case 2 ... 11:
			return &(core->regs.gen_reg[eff_reg - 2]);
		case 12:
			return &core->regs.pc;
			break;
		case 13:
			return &core->regs.ir;
			break;
		case 14:
			return &core->regs.sp;
		case 15:
			return &core->regs.bp;
		default:
			send_error(reg_invalid);
	}
}