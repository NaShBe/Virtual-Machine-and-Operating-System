/*	=========================================
 * architecture.c - implementation of the main VM architecture
 * 
 * this header should be used for documentation, but as for now it will hold temporary rules for the code:
 * 		1) any VM system errors or C program errors should abort the program
 * 		2) any errors created directly by VM user/programmer or VM programs should invoke VM system error handling (i.e. interrupts)
 * 
 * 
 * 
 * 
 * 
 * 
 *	========================================= 
 */

#include "architecture.h"
#include "instructions.h"
#include "handler.h"
#include <stddef.h>		/* for NULL */
#include <stdlib.h>		/* for malloc */
#include <pthread.h>	/* for threading of core and DMA processes */


volatile arch_byte	arch_memory[RAM_SIZE * ARCH_WORD_SIZE];
static arch_dma		core_dma_cont;
static arch_uint	core_amount;
static arch_bool	has_bus_control;
static const arch_interrupt_table const* intrpt_vector = &arch_memory + INTRPT_OFFSET;


// externally available functions
arch_core*	init_core_default   ();
arch_core*	init_core   		(arch_registers*, arch_pipe_func*, arch_addr);
void      	cycle				(arch_core**, arch_uint);
void		thread				(arch_core*, arch_addr);
arch_addr	connect_dma			(arch_device*);


// interally available functions
static void step		(arch_core*);
static void fetch		(arch_core*);
static void decode		(arch_core*);
static void no_op		(arch_core*);
static void read		(arch_core*);
static void write		(arch_core*);
static void store		(arch_core*);
static void load		(arch_core*);
static void arithm		(arch_core*);
static void mov_imm		(arch_core*);
static void add_imm		(arch_core*);
static void mul_imm		(arch_core*);
static void div_imm		(arch_core*);
static void load_imm	(arch_core*);
static void set_less_imm(arch_core*);
static void halt		(arch_core*);
static void jump		(arch_core*);
static void branch		(arch_core*);
static void interrupt	(arch_core*);

// helper functions
static void       	help_write_to_mem	(arch_byte*, arch_uint, arch_addr);
static arch_byte*	help_get_ram_addr	(arch_addr);
static arch_word*	help_get_reg		(arch_core*, arch_uint);
static void 		help_push			(arch_word, arch_core*);
static void			help_pop			(arch_core*);

arch_core* init_core_default()
{
	arch_core* core = malloc(sizeof(arch_core));
	if (core == NULL)
	{
		send_error(bad_malloc);
	}
	*core = (arch_core){.regs = {0}, .pipeline = {fetch, decode, no_op}};
	core->regs.pc = &arch_memory;
	core->cycle_count = 0;
	core_amount++;
	return core;
}

arch_core* init_core(arch_registers* regs_init, arch_pipe_func* pipeline, arch_addr entry_point)
{
	arch_core* core = malloc(sizeof(arch_core));
	if (core == NULL)
	{
		send_error(bad_malloc);
	}
	*core = (arch_core){.regs = *regs_init, .pipeline = pipeline};
	core->regs.pc = &arch_memory + entry_point;
	core->cycle_count = 0;
	core_amount++;
	return core;
}

void cycle(arch_core** core_list, arch_uint core_list_size)
{
	for (arch_int i = 0; i < core_list_size; i++)
	{
		arch_core* core = core_list[i];
		pthread_create(&core->thread, NULL, step, core);
		pthread_join(&core->thread, NULL);
	}
}

void thread(arch_core* core, arch_addr entry)
{

}

arch_addr connect_dma(arch_device* device)
{
	
}

void step (arch_core* core)
{
	do
	{
		if (CORE_PARALLEL)
		{
			if (core->cycle_count >= CORE_STEPS) core->cycle_count = CORE_STEPS-1;
			for(arch_uint i = core->cycle_count; i < CORE_STEPS; i++)
			{
				for(arch_uint j = i; j >= 0; j--)
				{
					core->pipeline[j](core);
				}
			}
			core->cycle_count++;
		}
		else
		{
			for (arch_uint i = 0; i < CORE_STEPS; i++)
			{
				core->pipeline[i](core);
			}
		}
	} while ((core->pipeline[CORE_EXE_STEP] != halt) || core->id == 0);
}

static void fetch(arch_core* core)
{
	core->regs.ir.int_rep = *((arch_word*)&arch_memory[core->regs.pc]);
	core->regs.pc++;
}

static void decode(arch_core* core)
{
	switch (core->regs.ir.format)
	{
		case FORMAT_AIF:
			if (((arith_data)core->regs.ir.data).zr != 0)
			{
				core->regs.pc = INTRPT_OFF_UNDEF;
				core->pipeline[CORE_EXE_STEP] = interrupt;
				return;
			}
			switch (core->regs.ir.opcode)
			{
				case SLT: case MOV ... OR:
					core->alu.instr = core->regs.ir.opcode;
					core->alu.oprand1 = ((arith_data)core->regs.ir.data).src1;
					core->alu.oprand2 = ((arith_data)core->regs.ir.data).src2;
					core->alu.output = help_get_reg(core, ((arith_data)core->regs.ir.data).dest);
					core->pipeline[CORE_EXE_STEP] = arithm;
					return;
				default:
					core->regs.pc = INTRPT_OFF_UNDEF;
					core->pipeline[CORE_EXE_STEP] = interrupt;
					return;
			}
			return;
		case FORMAT_CBIF:
			switch (core->regs.ir.opcode)
			{
				case ST:
					core->pipeline[CORE_EXE_STEP] = store;
					return;
				case LW:
					core->pipeline[CORE_EXE_STEP] = load;
					return;
				case MOVI:
					core->pipeline[CORE_EXE_STEP] = mov_imm;
					return;
				case ADDI:
					core->pipeline[CORE_EXE_STEP] = add_imm;
					return;
				case MULI:
					core->pipeline[CORE_EXE_STEP] = mul_imm;
					return;
				case DIVI:
					core->pipeline[CORE_EXE_STEP] = div_imm;
					return;
				case LDI:
					core->pipeline[CORE_EXE_STEP] = load_imm;
					return;
				case SLTI:
					core->pipeline[CORE_EXE_STEP] = set_less_imm;
					return;
				case BEQ: case BNE: case BEZ: case BNZ: case BGZ: case BLZ:
					if (((cond_imm_data)core->regs.ir.data).addr != 0 && ((cond_imm_data)core->regs.ir.data).dreg != 0)
					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = branch;
					return;
			}
			return;
		case FORMAT_UJF:
			switch (core->regs.ir.opcode)
			{
				case HTL:
					core->pipeline[CORE_EXE_STEP] = halt;
					return;
				case JMP:
					core->pipeline[CORE_EXE_STEP] = jump;
					return;
				case SWE:
					help_push(core->regs.pc, core);
					core->regs.pc = INTRPT_OFF_SOFT;
					core->pipeline[CORE_EXE_STEP] = interrupt;
					return;
				case NOP:
					core->pipeline[CORE_EXE_STEP] = no_op;
					return;
				default:
					help_push(core->regs.pc, core);
					core->regs.pc = INTRPT_OFF_UNDEF;
					core->pipeline[CORE_EXE_STEP] = interrupt;
					return;
			}
			break;
		case FORMAT_IOIF:
			switch (core->regs.ir.opcode)
			{
				case RD:
					core->pipeline[CORE_EXE_STEP] = read;
					return;
				case WR:
					core->pipeline[CORE_EXE_STEP] = write;
					return;
				default:
					help_push(core->regs.pc, core);
					core->regs.pc = INTRPT_OFF_UNDEF;
					core->pipeline[CORE_EXE_STEP] = interrupt;
					return;
			}
			return;
		default:
			help_push(core->regs.pc, core);
			core->regs.pc = INTRPT_OFF_UNDEF;
			core->pipeline[CORE_EXE_STEP] = interrupt;
			return;
	}
}

static void no_op(arch_core* core)
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
	arch_uint eff_addr;
	if (((cond_imm_data)core->regs.ir.data).dreg == 0)
	{
		eff_addr = ((cond_imm_data)core->regs.ir.data).addr;
	}
	else
	{
		eff_addr = ((cond_imm_data)core->regs.ir.data).dreg;
	}

	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	arch_word* reg = help_get_reg(core, eff_reg);
	eff_addr = *help_get_reg(core, eff_addr);

	if (reg != NULL)
	{
		help_write_to_mem(reg, sizeof(arch_word), eff_addr);
	}
	else 
	{
		send_error(reg_invalid);
	}
}

static void load(arch_core* core)
{
	arch_uint eff_addr;
	if (((cond_imm_data)core->regs.ir.data).dreg == 0)
	{
		eff_addr = ((cond_imm_data)core->regs.ir.data).addr;
	}
	else
	{
		eff_addr = ((cond_imm_data)core->regs.ir.data).dreg;
	}

	arch_word* temp = help_get_reg(core, eff_addr);
	eff_addr = *temp;
	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	if (eff_addr + 4 < RAM_SIZE*4-1)
	{
		*help_get_reg(core, eff_reg) = arch_memory[eff_addr];
	}
	else
	{
		send_error(ram_outbounds);
	}
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
		case AND:
			core->alu.output = core->alu.oprand1 & core->alu.oprand2;
			break;
		case OR:
			core->alu.output = core->alu.oprand1 | core->alu.oprand2;
			break;
		case SLT:
			break;
	}
	core->regs.ac = core->alu.output;
}

static void mov_imm(arch_core* core)
{
	
}

static void add_imm(arch_core* core)
{
	
}

static void mul_imm(arch_core* core)
{

}

static void div_imm(arch_core* core)
{

}

static void load_imm(arch_core* core)
{

}

static void set_less_imm(arch_core* core)
{

}

static void halt(arch_core* core)
{

}

static void jump(arch_core* core)
{
	core->regs.pc = core->regs.ir.data;
}

static void branch(arch_core* core)
{
	switch (core->regs.ir.opcode)
	{
		case BEQ:
			break;
		case BNE:
			break;
		case BEZ:
			break;
		case BNZ:
			break;
		case BGZ:
			break;
		case BLZ:
			break;
	}
}

static void interrupt(arch_core* core)
{
}

/*		HELPER FUNCTIONS		*/

static void help_write_to_mem(arch_byte* data, arch_uint size, arch_addr addr)
{
	arch_byte* offset = &(arch_memory[addr]);
	arch_uint count = 0;

	while (offset <= &(arch_memory[(RAM_SIZE * ARCH_WORD_SIZE)-1]) && size < count)
	{
		*offset = data[count];
		count++;
		offset++;
	}

	if (offset >= &(arch_memory[(RAM_SIZE * ARCH_WORD_SIZE)-1]))
	{
		send_error(ram_outbounds);
	}
}

static void help_write_to_mem(arch_word word, arch_addr addr)
{
	help_write_to_mem((arch_byte*)&word, ARCH_WORD_SIZE, addr);
}

static arch_word* help_get_reg(arch_core* core, arch_uint eff_reg)
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
		case 13:
			return &core->regs.ir;
		case 14:
			return &core->regs.sp;
		case 15:
			return &core->regs.bp;
		default:
			send_error(reg_invalid);
	}
}

static arch_byte* help_get_ram_addr(arch_addr address)
{
	return (arch_byte*)&arch_memory + address;
}

static void help_push(arch_word value, arch_core* core)
{
	arch_byte* temp = (arch_byte*)(&arch_memory + core->regs.sp);
	*(arch_word*)temp = value;
	core->regs.sp += ARCH_WORD_SIZE;
}

static void help_pop(arch_core* core)
{
	arch_word* temp = (arch_word*)((arch_byte*)&arch_memory + core->regs.sp);
	core->regs.ac = *temp;
	core->regs.sp -= ARCH_WORD_SIZE;
	if (core->regs.sp == core->regs.bp)
	{
		core->regs.bp = *(&arch_memory + core->regs.bp);
		core->regs.sp -= ARCH_WORD_SIZE;
	}
}
	/* architecture.c*/