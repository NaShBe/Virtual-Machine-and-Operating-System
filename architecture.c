#include "architecture.h"
#include "instructions.h"

typedef struct
{
	arch_addr reset;
	arch_addr undefined;
	arch_addr software;
} interrupt_table;

typedef struct
{

} PCB;

registers cpu_regs __attribute__((section("nberaki_cpu_regs")));
static char ram[1024 * 4] __attribute__((section("nberaki_ram")));
static char disk[2048 * 4] __attribute__((section("nberaki_disk")));

void init_cpu()
{
	cpu_regs.pc = &ram;
}

static void decode()
{
	switch GET_FORMAT(cpu_regs.ir.instr_data)
	{
		case FORMAT_AIF:
			switch (GET_OPCODE(cpu_regs.ir.instr_data))
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
					return;		//TODO
			}
			break;
		case FORMAT_CBIF:
			switch (GET_OPCODE(cpu_regs.ir.instr_data))
			{
				case 0:			//TODO
					break;
			}
			break;
		case FORMAT_UJF:
			switch (GET_OPCODE(cpu_regs.ir.instr_data))
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
			switch (GET_OPCODE(cpu_regs.ir.instr_data))
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
	execute(GET_OPCODE(cpu_regs.ir.instr_data));
}

static inline void fetch()
{
	cpu_regs.ir.instr_data = cpu_regs.pc;
	cpu_regs.pc++;
}

static void execute(unsigned char opcode)
{
	switch (opcode)
	{
		
	}
}