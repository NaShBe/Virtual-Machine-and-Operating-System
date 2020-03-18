#include "instructions.h" // arch_instr needed for definition in arch_core
//include <pthread.h>	// pthread_t needed for definition of arch_core

#ifndef VMOS_ARCH_H
#define VMOS_ARCH_H

#ifndef RAM_SIZE
#define RAM_SIZE	1024
#endif

#ifndef DISK_SIZE
#define DISK_SIZE	2048
#endif

#define ARCH_BYTE_SIZE	sizeof(arch_byte)
#define ARCH_HWORD_SIZE sizeof(arch_hword)
#define ARCH_WORD_SIZE	sizeof(arch_word)
#define ARCH_DWORD_SIZE sizeof(arch_dword)

#define ARCH_INT_SIZE		sizeof(arch_int)
#define ARCH_UINT_SIZE		sizeof(arch_uint)
#define ARCH_CHAR_SIZE		sizeof(arch_char)
#define ARCH_UCHAR_SIZE		sizeof(arch_uchar)
#define ARCH_FLOAT_SIZE		sizeof(arch_float)
#define ARCH_DOUBLE_SIZE	sizeof(arch_double)
#define ARCH_BOOL_SIZE		sizeof(arch_bool)

#define ARCH_ADDR_NULL	0

/* Interrupt Vector Address and Interrupt Offsets */
#define INTRPT_OFFSET 	0x0 /* interrupt vector starts at 0x0*/
#define INTRPT_SIZE  	0x18 /* interrupt vector size */

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
#define CORE_MAX_DEVICES	1
// NOTE: execution step must always be the last step (probably not true now)

#ifdef __WIN32
#include <windows.h>
typedef HANDLE arch_thread;
#elif defined(__APPLE__) || defined(unix) || defined(__unix__) || defined(__unix)
#include <pthread.h>	// pthread_t needed for definition of arch_core
typedef pthread_t arch_thread;
#endif

typedef unsigned char  			arch_byte;
typedef unsigned short 			arch_hword;
typedef unsigned int   			arch_word;
typedef unsigned long  			arch_dword;
typedef unsigned int   			arch_addr;
typedef char					arch_char;
typedef unsigned char			arch_uchar;
typedef int            			arch_int;
typedef unsigned int   			arch_uint;
typedef long           			arch_long;
typedef unsigned long  			arch_ulong;
typedef float          			arch_float;
typedef double         			arch_double;
typedef unsigned char   		arch_bool;

typedef struct a_r				arch_registers;
typedef struct a_i_t			arch_interrupt_table;
typedef struct a_alu			arch_alu;
typedef struct a_c				arch_core;
typedef struct a_dma_r			arch_dma_registers;
typedef struct a_d				arch_device;
typedef struct a_dma			arch_dma;

#include "instructions.h"

typedef void(* arch_pipe_func)(arch_core*);

#undef TRUE
#undef FALSE
#define TRUE  1
#define FALSE 0

typedef struct 
{
	arch_word   ac;				/* accumulator */
	arch_word   zr;			    /* zero register */
	arch_word   gen_reg[10];	/* general-purpose registers */
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
	arch_uint		id;
	arch_registers  regs;
	arch_alu        alu;
	arch_pipe_func	pipeline[CORE_STEPS];
	arch_word       cycle_count;
	arch_bool       is_enabled_intrpt;
	arch_thread		thread;
	arch_byte*		cache[50];
};

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
	arch_uint			device_count;
	arch_device**       devices;
	arch_addr			active_device;
	const arch_word    	size;
	arch_bool*			bus_access;
} arch_dma;

typedef struct
{
	arch_addr address;
	arch_addr* access;
} arch_device;

extern volatile arch_byte arch_memory[RAM_SIZE * ARCH_WORD_SIZE];

extern arch_core*   init_core_default   	();                         /* will initialize a core for use in cycle() */
extern arch_core*   init_core   			(arch_registers*, arch_pipe_func*, arch_addr);
extern void         cycle       			(arch_core**, arch_uint);   /* will cycles through every core in the list */
extern void         thread      			(arch_core*, arch_addr);    /* will jump core into process entry point*/
extern arch_addr    connect_dma 			(arch_device*);				/* connects a device and provides the address for programming */

extern void			help_write_to_mem		(arch_byte*, arch_uint, arch_addr);
extern void 		help_write_to_mem_word	(arch_word*, arch_addr);
arch_byte*			help_get_ram_addr		(arch_addr);
arch_addr			help_get_arch_addr		(arch_byte*);


extern void			write_to_memory		();							/* utility to write directly onto memory (USE SPARINGLY) */

#endif /* architecture.h */