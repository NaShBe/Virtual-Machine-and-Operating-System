#include "parse.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
    arch_uint job_num;
    arch_uint size;
    arch_uint priority;
} job_descriptor;

typedef struct
{
    arch_uint input_buff;
    arch_uint output_buff;
    arch_uint temp_buff;
} data_descriptor;

static FILE* input_file;
static FILE* context_file;

job_list* parse_file(const char*);
void output_context(job_list*);
void read_file(const char*);

job_list* parse_file(const char* path)
{
    read_file(path);
    arch_int read_stat = 0;

    arch_char*          field_type;
    job_descriptor*     job = NULL;
    data_descriptor*    data = NULL;
    
    job_list* jobs = malloc(sizeof(job_list));
    if (jobs == NULL)
    {
        send_error(bad_malloc);
    }
    jobs->capacity = PARSE_INIT_CAPACITY;
    jobs->size = 0;
    jobs->jobs = NULL;

    while (!feof(input_file))
    {
        read_stat = fscanf(input_file, "%s", field_type);
        if (read_stat == 0)
        {
            send_error(parser_file_error);
        }

        if (field_type = "//")
        {
            arch_uint field1;
            arch_uint field2;
            arch_uint field3;

            read_stat = fscanf(input_file, "%s %i %i %i", field_type, &field1, &field2, &field3);
            if (feof(input_file))
            {
                break;
            }
            field_type = help_to_lower_string(field_type);

            if (strcmp(field_type, "job") == 0)
            {
                if (job == NULL)
                {
                    job = malloc(sizeof(job_descriptor));
                    if (job == NULL)
                    {
                        send_error(bad_malloc);
                    }
                    job->job_num = field1;
                    job->size = field2;
                    job->priority = field3;
                }
                else
                {
                    send_error(parser_file_error);
                }
            }

            else if (strcmp(field_type, "data") == 0)
            {
                if (data == NULL && job != NULL)
                {

                }

                else
                {
                    send_error(parser_file_error);
                }
                
            }

            else if (strcmp(field_type, "end") == 0)
            {
                if (data == NULL || job == NULL)
                {
                    send_error(parser_file_error);
                }

                else
                {
                    
                }
            }
        }
        else if (field_type[0] == '0' && field_type[1] == 'x')
        {
            if (strcmp(field_type, "job") == 0 && job != NULL)
            {
                for (arch_int i = 0; i < job->size; i++)
                {
                    
                }
            }
        }
    }
    fclose(input_file);
}

void output_context(job_list* info)
{

}

void read_file(const char* path)
{
    input_file = fopen(path, "r");
    if (input_file == NULL)
    {
        send_error(parser_file_error);
    }
}

parse_job* help_create_job(arch_uint priority, arch_uint i_size, arch_instr* instr, arch_uint d_size, arch_word* data)
{
    parse_job* job = malloc(sizeof(parse_job));
    if (job == NULL)
    {
        send_error(bad_malloc);
    }

    job->priority = priority;
    job->instr_size = i_size;
    job->instructions = instr;
    job->data_size = d_size;
    job->data = data;
    return job;
}

void help_add_to_jobs(parse_job* job)
{
    
}

arch_instr help_parse_to_instr(arch_uint data)
{
    arch_instr instruction;
    switch (ARCH_WORD_SIZE)
    {
    case 4:
        instruction.format = (data & BITFIELD_FORMAT) >> BITOFFSET_FORMAT;
        instruction.opcode = (data & BITOFFSET_OPCODE) >> BITOFFSET_OPCODE;
        break;
    default:
        send_error(parser_file_error);
        break;
    }
    return instruction;
}

arch_char* help_to_lower_string(arch_char* string)
{

}