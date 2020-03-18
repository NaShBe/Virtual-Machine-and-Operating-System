#ifndef VMOS_VMOS_H
#define VMOS_VMOS_H

typedef unsigned char   vmos_byte;
typedef unsigned char   vmos_uchar;
typedef char            vmos_char;
typedef unsigned short  vmos_ushort;
typedef short           vmos_short;
typedef unsigned int    vmos_uint;
typedef int             vmos_int;
typedef unsigned int    vmos_bool;
#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0
typedef struct vmos_pcb vmos_pcb;

extern vmos_bool      is_done_executing;


#endif