#ifndef VMOS_INSTRUCTIONS_H
#define VMOS_INSTRUCTIONS_H

// Format Defines
// first two bits of instruction, determines type
#define SIZE_FORMAT 	2
#define OFFSET_FORMAT	0
#define FORMAT_AIF  	0x00	// Arithmatic Instruction Format
#define FORMAT_CBIF 	0x01	// Conditional Branch & Immediate Instruction Format
#define FORMAT_UJF  	0x02	// Unconditional Jump Format
#define FORMAT_IOIF 	0x03	// I/O Instruction Format

#define SIZE_OPCODE 	6
#define OFFSET_OPCODE	4
#define RD   0x00	// Load contents of input buffer into the accumulator
#define WR   0x01	// Write the contents of accumulator into output buffer
#define ST   0x02	// Store contents of a register into RAM
#define LW   0x03	// Load contents from memory into a register
#define MOV  0x04	// Copy content of one register into another
#define ADD  0x05	// Add source register content into destination register
#define SUB  0x06	// Subtract source register content into destination register
#define MUL  0x07	// Copy the product of two source registers into a destination register
#define DIV  0x08	// Copy the quotient of two source registers into a destination register
#define AND  0x09	// Copy the logical AND of two source registers into a destination register
#define OR   0x0A	// Copy the logical OR of two source registers into a destination register
#define MOVI 0x0B	// Copy an immediate value directly into a register
#define ADDI 0x0C	// Add an immediate value directly into the contents of a register
#define MULI 0x0D	// Multiples an immediate value directly into the contents of a register
#define DIVI 0x0E	// Divides
#define LDI  0x0F	//
#define SLT  0x10	//
#define SLTI 0x11	//
#define HTL  0x12	// Marks the end of a program and halts
#define NOP  0x13	// No operation performed, program continues with next instruction
#define JMP  0x14	// Jump to a specific location in memory
#define BEQ  0x15	// Branch to an address if the source and destination register contents are equal
#define BNE  0x16	// Branch to an address if the source and destination register contents are NOT equal
#define BEZ  0x17	// Branch to an address if
#define BNZ  0x18	// Branch to an address if
#define BGZ  0x19	// Branch to an address if
#define BLZ  0x1A	// Branch to an address if
#define SWE  0x1B	// Software Interrupt

#define SIZE_DATA 24

#define BITFIELD_FORMAT 		0xC0000000
#define BITOFFSET_FORMAT		30
#define BITFIELD_OPCODE			0x3F000000
#define BITOFFSET_OPCODE		24
#define BITFIELD_OPRAND1		0x00F00000
#define BITFIELD_OPRAND2		0x000F0000
#define BITFIELD_OPRAND3		0x0000F000
#define BITFIELD_ADDRESS		0x0000FFFF
#define BITFIELD_ARITHZERO		0x00000FFF

#include "architecture.h" // for arch_uint

typedef union
{
	arch_uint int_rep;
	struct
	{
		arch_uint format: 2;
		arch_uint opcode: 6;
		arch_uint data:  24;
	};
} arch_instr;

typedef union
{
	arch_uint int_rep: 24;		// uint-compatable representation
	struct
	{
		arch_uint src1: 4;	
		arch_uint src2: 4;
		arch_uint dest: 4;
		const arch_uint zr: 12;	// the last 12 bits must be zero
	};
} arith_data;

typedef union
{
	arch_uint int_rep: 24;		// uint-compatable representation
	struct
	{
		arch_uint breg: 4;
		arch_uint dreg: 4;
		arch_uint addr: 16;
	};
} cond_imm_data;

typedef union
{
	arch_uint int_rep: 24;		// uint-compatable representation
	struct
	{
		arch_uint reg1: 4;
		arch_uint reg2: 4;
		arch_uint addr: 16;
	};
} inp_out_data;

typedef struct 
{
	arch_uint	size;
	arch_uint 	capacity;
	arch_instr*	instructions;
}instruction_list;



#endif
