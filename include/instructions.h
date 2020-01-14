#ifndef NBERAKI_INSTRUCTIONS_H
#define NBERAKI_INSTRUCTIONS_H

// Format Defines
// first two bits of instruction, determines type
#define FORMAT_AIF  0x00	//
#define FORMAT_CBIF 0x01	//
#define FORMAT_UJF 0x02	//
#define FORMAT_IOIF 0x03	// I/O type, 

#define RD 0x00		// Load contents of input buffer into the accumulator
#define WR 0x01		// Write the contents of accumulator into output buffer
#define ST 0x02		// Store contents of a register into RAM
#define LW 0x03		// Load contents from memory into a register
#define MOV 0x04	// Copy content of one register into another
#define ADD 0x05	// Add source register content into destination register
#define SUB 0x06	// Subtract source register content into destination register
#define MUL 0x07	// Copy the product of two source registers into a destination register
#define DIV 0x08	// Copy the quotient of two source registers into a destination register
#define AND 0x09	// Copy the logical AND of two source registers into a destination register
#define OR 0x0A		// Copy the logical OR of two source registers into a destination register
#define MOVI 0x0B	// Copy an immediate value directly into a register
#define ADDI 0x0C	// Add an immediate value directly into the contents of a register
#define MULI 0x0D	// Multiples an immediate value directly into the contents of a register
#define DIVI 0x0E	// Divides
#define LDI 0x0F	//
#define SLT 0x10	//
#define SLTI 0x11	//
#define HTL 0x12	// Marks the end of a program and halts
#define NOP 0x13	// No operation performed, program continues with next instruction
#define JMP 0x14	// Jump to a specific location in memory
#define BEQ 0x15	// Branch to an address if the source and destination register contents are equal
#define BNE 0x16	// Branch to an address if the source and destination register contents are NOT equal
#define BEZ 0x17	// Branch to an address if
#define BNZ 0x18	// Branch to an address if
#define BGZ 0x19	// Branch to an address if
#define BLZ 0x1A	// Branch to an address if

#define GET_FORMAT(x) ((0xC0000000 & x) >> 30) 
#define GET_OPCODE(x) ((0x3F000000 & x) >> 24)

typedef union
{
	unsigned int instr_data;
	struct fields
	{
		unsigned int type: 2;
		unsigned int opcode: 6;
		unsigned int data: 24;
	};
}instruction_t;

typedef union
{
	unsigned int data: 24;
	struct fields
	{
		unsigned int src1: 4;
		unsigned int src2: 4;
		unsigned int dest: 4;
		const unsigned int zr: 12;
	};
} arith_data;

typedef union
{
	unsigned int data: 24;
	struct fields
	{
		unsigned int breg: 4;
		unsigned int dreg: 4;
		unsigned int addr: 16;
	};
} cond_imm_data;

typedef union
{
	unsigned int data: 24;
	struct fields
	{
		unsigned int reg1: 4;
		unsigned int reg2: 4;
		unsigned int addr: 16;
	};
};

#endif