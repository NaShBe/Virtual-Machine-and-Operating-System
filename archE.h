#ifndef NBERAKI_ARCHE_H
#define NBERAKI_ARCHE_H

#define ARCHE_MAGIC_NUM 0xACEAC000

typedef struct
{
	unsigned int magic_num;		
	unsigned int text;
	unsigned int data;
} archE_header;

#endif
