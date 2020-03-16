#ifndef VMOS_PARSE_H
#define VMOS_PARSE_H

#define PARSED_SUCCESS 0
#define PARSED_FAILURE 1

#define PARSE_INIT_CAPACITY 30

#include "architecture.h"

typedef struct
{
    arch_uint   priority;
    arch_uint   instr_size;
    arch_instr* instructions;
    arch_uint   data_size;
    arch_word*  data;
} parse_data;

typedef struct
{
    arch_uint   size;
    arch_uint   capacity;
    parse_data* jobs;
} parse_jobs;


#endif