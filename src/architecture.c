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

#include "vmos.h"
#include "architecture.h"
#include "instructions.h"
#include "handler.h"
#include "process.h"
#include "harddrive.h"
#include <stddef.h>		/* for NULL */
#include <stdlib.h>		/* for malloc */
#include <stdio.h>		/* for printf */
#include <string.h>		/* for memcpy */
#include <pthread.h>	/* for threading of core and DMA processes */


volatile arch_byte		arch_memory[RAM_SIZE * ARCH_WORD_SIZE];
static arch_dma*		core_dma_cont = NULL;
static arch_uint		core_amount;
static arch_bool		has_bus_control;
static arch_interrupt_table* const intrpt_vector = (arch_interrupt_table*)(arch_memory + INTRPT_OFFSET);


// externally available functions
arch_core*	init_core_default   ();
arch_core*	init_core   		(arch_registers*, arch_pipe_func*, arch_addr);
void      	cycle				(arch_core**, arch_uint);
void		rerun				(arch_core*);
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
void       			help_write_to_mem		(arch_byte*, arch_uint, arch_addr);
void 				help_write_to_mem_word	(arch_word*, arch_addr);
arch_byte*			help_get_ram_addr		(arch_addr);
arch_addr			help_get_arch_addr		(arch_byte*);
arch_uint			help_get_instruction	(arch_core*);
static arch_word*	help_get_reg			(arch_core*, arch_uint);
static void 		help_push				(arch_word, arch_core*);
static void			help_pop				(arch_core*);
void 				help_write_instr_to_mem	(arch_instr* instr, arch_uint size, arch_addr addr);

arch_core* init_core_default()
{
	const arch_instr resetjump = {.format = FORMAT_UJF, .opcode = JMP, .data = help_get_arch_addr((arch_byte*) intrpt_vector)};
	*intrpt_vector = (arch_interrupt_table){.reset = resetjump, .undefined = 0, .math = 0, .dma = 0, .software = 0};

	arch_core* core = malloc(sizeof(arch_core));
	if (core == NULL)
	{
		send_error(bad_malloc);
	}
	*core = (arch_core){.regs = {0}, .pipeline = {fetch, decode, no_op}};
	core->regs.pc = 0;
	core->cycle_count = 0;
	core_amount++;
	core->pcb_reference = NULL;
	return core;
}

arch_core* init_core(arch_registers* regs_init, arch_pipe_func* pipeline, arch_addr entry_point)
{
	arch_core* core = malloc(sizeof(arch_core));
	if (core == NULL)
	{
		send_error(bad_malloc);
	}
	*core = (arch_core){.regs = *regs_init};
	for (arch_int i = 0; i < CORE_STEPS; i++)
	{
		core->pipeline[i] = pipeline[i];
	}
	core->regs.pc = help_get_arch_addr((arch_byte*) arch_memory + entry_point);
	core->cycle_count = 0;
	core_amount++;
	core->pcb_reference = NULL;
	return core;
}

void cycle(arch_core** core_list, arch_uint core_list_size)
{
	#ifdef __WIN32
	// code for windows threads
	for (arch_int i = 0; i < core_list_size; i++)
    {
        arch_core* core = core_list[i];
        CreateThread(NULL,0,step,core,0,&core->thread);
        WaitForSingleObject(&core->thread, INFINITE);
    }
	#elif defined(__APPLE__) || defined(unix) || defined(__unix__) || defined(__unix)
	for (arch_int i = 0; i < core_list_size; i++)
	{
		arch_core* core = core_list[i];
		core->thread = malloc(sizeof(thread));
		pthread_create(&core->thread, NULL, step, core);
		//pthread_join(core->thread, NULL);		These programs should work concurrently.
	}
	#endif
}

void rerun (arch_core* core)
{
	core->pipeline[CORE_EXE_STEP-1] = no_op;
	core->regs.pc -= ARCH_WORD_SIZE;
}

void thread(arch_core* core, arch_addr entry)
{

}

void thread_cache(vmos_pcb* process)
{
	
}

arch_addr connect_dma(arch_device* device)
{
	if (core_dma_cont == NULL)
	{
		core_dma_cont = malloc(sizeof(arch_dma));
		if (core_dma_cont == NULL)
		{
			send_error(bad_malloc);
		}
		core_dma_cont->devices = malloc(sizeof(arch_device) * CORE_MAX_DEVICES);
		if(core_dma_cont->devices == NULL)
		{
			send_error(bad_malloc);
		}
	}
	if (core_dma_cont->device_count >= CORE_MAX_DEVICES)
	{
		printf("Max devices reached, device not connected.");
		return 0;
	}
	else
	{
		core_dma_cont->devices[core_dma_cont->device_count] = device;
		core_dma_cont->device_count++;
		device->address = core_dma_cont->device_count;
		return core_dma_cont->device_count;
	}
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

		
	} while (is_done_executing == FALSE);
}

static void fetch(arch_core* core)
{
	if (core->regs.pc < RAM_SIZE * ARCH_WORD_SIZE)
	{
		core->regs.ir.int_rep = *(arch_word*)(arch_memory + core->regs.pc);
		//printf("0x%x\n", core->regs.ir.int_rep);
		core->regs.pc += 4;
	}
	else
	{
		send_error(ram_outbounds);
	}
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
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = mov_imm;
					return;
				case ADDI:
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = add_imm;
					return;
				case MULI:
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = mul_imm;
					return;
				case DIVI:
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = div_imm;
					return;
				case LDI:
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
					core->pipeline[CORE_EXE_STEP] = load_imm;
					return;
				case SLTI:
					if (((cond_imm_data)core->regs.ir.data).dreg != 0)
 					{
						core->regs.pc = INTRPT_OFF_UNDEF;
						core->pipeline[CORE_EXE_STEP] = interrupt;
					}
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
	arch_addr data_address;
	if (((inp_out_data)core->regs.ir.data).addr == 0)
	{
		data_address = ((inp_out_data)core->regs.ir.data).reg2;
	}
	else
	{
		data_address = ((inp_out_data)core->regs.ir.data).addr;
	}

	*(help_get_reg(core, ((inp_out_data)core->regs.ir.data).reg1)) = read_data(core->pcb_reference->fd, data_address);
}

static void write(arch_core* core)
{
	arch_addr data_address;
	if (((inp_out_data)core->regs.ir.data).addr == 0)
	{
		data_address = ((inp_out_data)core->regs.ir.data).reg2;
	}
	else
	{
		data_address = ((inp_out_data)core->regs.ir.data).addr;
	}

	write_data(core->pcb_reference->fd, data_address, *(help_get_reg(core, ((inp_out_data)core->regs.ir.data).reg1)));
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
		help_write_to_mem((arch_byte*)reg, sizeof(arch_word), eff_addr);
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
			*core->alu.output = core->alu.oprand1 & core->alu.oprand2;
			break;
		case OR:
			*core->alu.output = core->alu.oprand1 | core->alu.oprand2;
			break;
		case SLT:
			break;
	}
	core->regs.ac = *core->alu.output;
}

static void mov_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
  arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
  *help_get_reg(core, eff_reg) = eff_data;
}

static void add_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	*help_get_reg(core, eff_reg) += eff_data;
}

static void mul_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
  	*help_get_reg(core, eff_reg) = *help_get_reg(core, eff_reg) * eff_data;
}

static void div_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
  	*help_get_reg(core, eff_reg) = *help_get_reg(core, eff_reg) / eff_data;
}

static void load_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
  	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg; 
  	*help_get_reg(core, eff_reg) = eff_data;
}

static void set_less_imm(arch_core* core)
{
	arch_uint eff_data = ((cond_imm_data)core->regs.ir.data).addr;
  	arch_uint eff_reg = ((cond_imm_data)core->regs.ir.data).breg;
	arch_uint d_reg = ((cond_imm_data)core->regs.ir.data).dreg;
	if(eff_reg < eff_data) 
	{
		*help_get_reg(core, d_reg) = 1;
	} 
	else
	{
		*help_get_reg(core, d_reg) = 0;
	}
}

static void halt(arch_core* core)
{
	core->regs.pc -= ARCH_WORD_SIZE;
}

static void jump(arch_core* core)
{
	core->regs.pc = core->regs.ir.data;
}

static void branch(arch_core* core)
{
	arch_addr jmp_addr;
	switch (core->regs.ir.opcode)
	{
		case BEQ:
			if (((cond_imm_data)core->regs.ir.data).breg == ((cond_imm_data)core->regs.ir.data).dreg)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
				return;
			}
			else {return;} 
			break;
		case BNE:
			if (((cond_imm_data)core->regs.ir.data).breg != ((cond_imm_data)core->regs.ir.data).dreg)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
			}
			else {return;} 
			break;
		case BEZ:
			if (((cond_imm_data)core->regs.ir.data).breg == core->regs.zr)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
			}
			else {return;} 

			break;
		case BNZ:
			if (((cond_imm_data)core->regs.ir.data).breg != core->regs.zr)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
			}
			else {return;} 
			break;
		case BGZ:
			if (((cond_imm_data)core->regs.ir.data).breg > core->regs.zr)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
			}
			else {return;} 
			break;
		case BLZ:
			if (((cond_imm_data)core->regs.ir.data).breg < core->regs.zr)
			{
				jmp_addr = ((cond_imm_data) core->regs.ir.data).addr;
				core->regs.pc = jmp_addr;
			}
			else {return;} 
			break;
	}
}

static void interrupt(arch_core* core)
{
	send_error(undefined);
}

/*		HELPER FUNCTIONS		*/

void help_write_to_mem(arch_byte* data, arch_uint size, arch_addr addr)
{
	volatile arch_byte* offset = help_get_ram_addr(addr);
	arch_uint count = 0;

	while (offset <= (arch_memory + (RAM_SIZE * ARCH_WORD_SIZE))-1 && size < count)
	{
		*offset = data[count];
		count++;
		offset++;
	}

	if (offset >= (arch_memory + (RAM_SIZE * ARCH_WORD_SIZE))-1)
	{
		send_error(ram_outbounds);
	}
}

void help_write_to_mem_word(arch_word* word, arch_addr addr)
{
	help_write_to_mem((arch_byte*)word, ARCH_WORD_SIZE, addr);
}

void help_write_instr_to_mem(arch_instr* instr, arch_uint size, arch_addr addr)
{
	if (addr%4 != 0)
	{
		return;
	}
	else
	{
		volatile arch_byte* offset = help_get_ram_addr(addr);
		if (offset + size < arch_memory + (RAM_SIZE * ARCH_WORD_SIZE))
		{
			memcpy((arch_byte*)arch_memory+addr, instr, size);
		}
	}
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
			return (arch_word*)&core->regs.ir;
		case 14:
			return &core->regs.sp;
		case 15:
			return &core->regs.bp;
		default:
			send_error(reg_invalid);
	}
	return NULL;
}

arch_byte* help_get_ram_addr(arch_addr address)
{
	return (arch_byte*)(arch_memory + address);
}

arch_addr help_get_arch_addr(arch_byte* address)
{
	if (address == NULL)
	{
		return 0;
	}
	return(arch_addr)(address - arch_memory);
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
		core->regs.bp = (arch_addr)*(arch_memory + core->regs.bp);
		core->regs.sp -= ARCH_WORD_SIZE;
	}
}

arch_uint help_get_instruction(arch_core* core)
{
	return core->regs.ir.opcode;
}
	/* architecture.c*/