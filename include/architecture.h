#ifndef VMOS_ARCH_H
#define VMOS_ARCH_H

#ifndef RAM_SIZE
#define RAM_SIZE 1024
#endif

#ifndef DISK_SIZE
#define DISK_SIZE 2048
#endif

#define ARCH_BYTE_SIZE sizeof(unsigned char)
#define ARCH_HWORD_SIZE sizeof(unsigned short)
#define ARCH_WORD_SIZE sizeof(unsigned int)
#define ARCH_DWORD_SIZE sizeof(unsigned long)

/* Interrupt Vector Address and Interrupt Offsets */
#define INTRPT_OFFSET 0x0 /* interrupt vector starts at 0x0*/
#define INTRPT_SIZE  0x18 /* interrupt vector size */

#define INTRPT_OFF_RESET INTRPT_OFFSET + 0x0  /* Reset the computer */
#define INTRPT_OFF_UNDEF INTRPT_OFFSET + 0x4  /* Undefined instruction */
#define INTRPT_OFF_MATH  INTRPT_OFFSET + 0x8  /* Mathematical issue (DIV by 0) */
#define INTRPT_OFF_DMA   INTRPT_OFFSET + 0xC  /* DMA controller */
#define INTRPT_OFF_STKU	 INTPRT_OFFSET + 0x10 /* Stack underflow */
#define INTRPT_OFF_SOFT  INTRPT_OFFSET + 0x14 /* Software interrupt for operating system */

#define DMA_REGISTERS_ADDRESS          	 0x1234 /* Address where DMA registers are located */

#define CORE_PARALLEL 0		// This establishes if instruction pipelining happens.
#define CORE_STEPS    3		// This is how many steps are taken to complete one CPU instruction (fetching, decoding, executing)
#define CORE_EXE_STEP CORE_STEPS - 1
// NOTE: execution step must always be the last step. (probably not true now)

typedef unsigned char   arch_byte;
typedef unsigned short  arch_hword;
typedef unsigned int    arch_word;
typedef unsigned long   arch_dword;
typedef unsigned int    arch_addr;
typedef int             arch_int;
typedef unsigned int    arch_uint;
typedef long            arch_long;
typedef unsigned long   arch_long;
typedef float           arch_float;
typedef double          arch_double;
typedef unsigned char   arch_bool;
typedef void(* arch_pipe_func)(arch_core*);


#undef TRUE
#undef FALSE
#define TRUE  1
#define FALSE 0

typedef struct 
{
	arch_word   ac;				/* accumulator */
	arch_word   zr;			    /* zero register */
	arch_word   gen_reg[10];		/* general-purpose registers */
	arch_addr   pc;				/* program counter */
	arch_instr  ir;				/* instruction register */
	arch_addr   sp;				/* stack pointer */
	arch_addr   bp;				/* base pointer */
} arch_registers;		/* instructions provide access to the CPU's 16 registers */

typedef struct
{
	arch_instr reset;
	arch_instr undefined;
	arch_instr math;
	arch_instr dma;
	arch_instr software;
} arch_interrupt_table;

typedef struct
{
	arch_word   oprand1;
	arch_word   oprand2;
	arch_word   instr;
	arch_word*  output;
} arch_alu;

typedef struct
{
	arch_registers  regs;
	arch_alu        alu;
	arch_pipe_func	pipeline[CORE_STEPS];
	arch_word       cycle_count;
	arch_bool       is_enabled_intrpt;
} arch_core;

typedef struct
{
	arch_addr address;
	arch_word count;
	arch_word intrpt;
	arch_word read;
	arch_word write;
} arch_dma_registers;

typedef struct
{
	arch_dma_registers* registers;
	arch_addr*          devices;
	arch_word           size;
} arch_dma;

extern volatile arch_byte core_ram[RAM_SIZE * ARCH_WORD_SIZE];

extern arch_core*   init_core_default   ();                         /* will initialize a core for use in cycle() */
extern arch_core*   init_core   		(arch_registers*, arch_pipe_func*, arch_addr);
extern void         cycle       		(arch_core**, arch_uint);   /* will cycles through every core in the list */
extern void         thread      		(arch_core*, arch_addr);    /* will jump core into process entry point*/
extern arch_addr    connect_dma 		(void(*)(arch_dma*));

#endif /* architecture.h */