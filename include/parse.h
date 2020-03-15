#ifndef VMOS_PARSE_H
#define VMOS_PARSE_H

#define VMOS_PARSED_SUCCESS 0
#define VMOS_PARSED_FAILURE 1

#include "architecture.h"

typedef struct
{
    arch_instr* instructions;
    arch_word*  data;
} parse_data;

#endif