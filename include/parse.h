#ifndef VMOS_PARSE_H
#define VMOS_PARSE_H

#define PARSED_SUCCESS 0
#define PARSED_FAILURE 1

#define PARSE_INIT_CAPACITY 30

#include "architecture.h"

typedef struct
{
    arch_uint   priority;
    arch_uint   instr_count;
    arch_instr* instructions;
    arch_uint   data_count;
    arch_word*  data;
} parse_job;

typedef struct
{
    arch_uint   count;
    arch_uint   capacity;
    parse_job** jobs;
} job_list;

typedef struct
{
    arch_uint job_num;
    arch_uint instr_count;
    arch_uint priority;
} job_descriptor;

typedef struct
{
    arch_uint input_buff_size;
    arch_uint output_buff_size;
    arch_uint temp_buff_size;
} data_descriptor;

extern job_list* parse_file(const char*);
void output_context(job_list*, const arch_char*);
extern void read_file(const char*);

#endif