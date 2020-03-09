#ifndef VMOS_ARCHE_H
#define VMOS_ARCHE_H

#include "vmos.h" // basic data structures for vmos

#define ARCHE_MAGIC_NUM 0xACEAC000

typedef vmos_uint	e_address;
typedef vmos_uint 	e_value;
typedef vmos_char*	e_buffer;

typedef struct
{
	e_value		magic_num; /* should always equal ARCHE_MAGIC_NUM */
	e_address	text;	   /* should hold the address where code starts */
	e_address	t_size;
	e_address	data;	   /* should hold the address where data starts */ 
	e_address	d_size;
} archE_header;

#endif /*archE.h*/
