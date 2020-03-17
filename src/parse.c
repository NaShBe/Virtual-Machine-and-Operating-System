#include "parse.h"
#include "handler.h"
#include "instructions.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_MAX        50
#define FIELD_OFFSET    0
#define HEADER_OFFSET   3

#define DATA_INPUT_BUFFER_SIZE  0x14
#define DATA_OUTPUT_BUFFER_SIZE 0xC
#define DATA_TEMP_BUFFER_SIZE   0xC
#define DATA_TOTAL_SIZE DATA_TEMP_BUFFER_SIZE + DATA_INPUT_BUFFER_SIZE + DATA_OUTPUT_BUFFER_SIZE

static FILE* input_file = NULL;
static FILE* context_file = NULL;

job_list* parse_file(const char*);
void output_context(job_list*, const arch_char*);
void read_file(const char*);

job_list* parse_file(const char* path)
{
    if (path != NULL)
    {
        read_file(path);
    }

    if (input_file == NULL)
    {
        send_error(parser_file_error);
    }

    arch_char*          read_line;
    job_descriptor*     job = NULL;
    data_descriptor*    data = NULL;
    
    job_list* jobs = malloc(sizeof(job_list));
    if (jobs == NULL)
    {
        send_error(bad_malloc);
    }
    jobs->capacity = PARSE_INIT_CAPACITY;
    jobs->count = 0;
    jobs->jobs = NULL;

    while (fgets(read_line, LINE_MAX, input_file) != NULL)
    {
        instruction_list* instr_list = NULL;                       
        arch_word*        data_list = NULL;                        
        arch_char* field_type = NULL;
        strlcpy(field_type, read_line, SIZE_FORMAT);        
        if (strcmp(field_type, "//") == 0)                  
        {
            arch_char* header_type;
            arch_char* field1;
            arch_char* field2;
            arch_char* field3;

            header_type = strtok(read_line, " ");
            if (header_type == NULL)
            {
                send_error(parser_file_error);
            }
            field1 = strtok(NULL, " ");
            if (field1 == NULL)
            {
                send_error(parser_file_error);
            }
            field2 = strtok(NULL, " ");
            if (field2 == NULL)
            {
                send_error(parser_file_error);
            }
            field3 = strtok(NULL, " ");
            if (field3 == NULL)
            {
                send_error(parser_file_error);
            }
            
            header_type = help_to_lower_string(header_type);

            if (strcmp(header_type, "job") == 0)
            {
                if (job == NULL && data == NULL)
                {
                    job = malloc(sizeof(job_descriptor));
                    if (job == NULL)
                    {
                        send_error(bad_malloc);
                    }
                    job->job_num = field1;
                    job->instr_count = field2;
                    job->priority = field3;

                    sscanf(field1, "%i", &job->job_num);
                    sscanf(field2, "%i", &job->instr_count);
                    sscanf(field3, "%i", &job->priority);
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
                    data = malloc(sizeof(data_descriptor));
                    if (data == NULL)
                    {
                        send_error(bad_malloc);
                    }
                    data->input_buff_size = DATA_INPUT_BUFFER_SIZE;
                    data->output_buff_size = DATA_OUTPUT_BUFFER_SIZE;
                    data->temp_buff_size = DATA_TEMP_BUFFER_SIZE;
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
                    parse_job* new_job = help_create_job(job, data, instr_list, data_list);
                    help_add_to_job_list(jobs, new_job);
                }
            }
        }
        else if (strcmp(field_type, "0x") == 0);
        {
            if (job != NULL && data == NULL)
            {
                instr_list = malloc(sizeof(arch_instr) * job->instr_count);
                if (instr_list == NULL)
                {
                    send_error(bad_malloc);
                }
                for (arch_int i = 0; i < job->instr_count; i++)
                {
                    sscanf(read_line, "%i", &instr_list[i]);
                }
            }

            else if (job != NULL && data != NULL)
            {
                data_list = malloc (sizeof(arch_word) * DATA_TOTAL_SIZE);
                if (data_list == NULL)
                {
                    send_error(bad_malloc);
                }
                for (arch_int i = 0; i < DATA_TOTAL_SIZE; i++)
                {
                    sscanf(read_line, "%i", &data_list[i]);
                }
            }
            else
            {
                send_error(parser_file_error);
            }
        }
    }
    fclose(input_file);
}

void output_context(job_list* info, const arch_char* file)
{
    FILE *fp;
    if( access( file, F_OK ) != -1 ) {
        // file exists
        fp = fopen(file, "w+");
    }
    else
    {
        // file doesn't exist
        fp = fopen(file, "w");
    }
    for (arch_int i = 0; i < info->count; i++)
    {
        fprintf(fp, "opcode: %i, priority of %i\n", i, info->jobs[i]->priority);
        for (arch_int j = 0; j < info->jobs[i]->instr_count; j++)
        {
            if (info->jobs[i]->instructions[j].format == FORMAT_AIF) // If the format is arithmetic instruction 
            { 
                fprintf(fp, "#%i opcode: %i src1: %i, src2: %i, dest: %i\n", j,
                        ((arch_instr)(info->jobs[i]->instructions[j])).opcode,
                        ((arith_data)(info->jobs[i]->instructions[j].data)).src1,
                        ((arith_data)(info->jobs[i]->instructions[j].data)).src2,
                        ((arith_data)(info->jobs[i]->instructions[j].data)).dest);
            }
            if (info->jobs[i]->instructions[j].format == FORMAT_CBIF) // If the format is Conditional Branch & Immediate Instruction instruction 
            {
                fprintf(fp, "#%i Conditional Branch or Immediate opcode: %i breg: %i, dreg: %i, addr: %i\n", j,
                        ((arch_instr)(info->jobs[i]->instructions[j])).opcode,
                        ((cond_imm_data)(info->jobs[i]->instructions[j].data)).breg,
                        ((cond_imm_data)(info->jobs[i]->instructions[j].data)).dreg,
                        ((cond_imm_data)(info->jobs[i]->instructions[j].data)).addr);
            }
            if (info->jobs[i]->instructions[j].format == FORMAT_UJF) // If the format is Unconditional Jump instruction 
            {
                fprintf(fp, "#%i opcode: %i data: %i\n", j,
                        ((arch_instr)(info->jobs[i]->instructions[j])).opcode,
                        ((arch_instr)(info->jobs[i]->instructions[j])).data);
            }
            if (info->jobs[i]->instructions[j].format == FORMAT_IOIF) // If the format is I/O instruction 
            {
                fprintf(fp, "#%i I/O opcode: %i: reg1: %i, reg2: %i, addr: %i\n", j,
                        ((arch_instr)(info->jobs[i]->instructions[j])).opcode,
                        ((inp_out_data)(info->jobs[i]->instructions[j].data)).reg1,
                        ((inp_out_data)(info->jobs[i]->instructions[j].data)).reg2,
                        ((inp_out_data)(info->jobs[i]->instructions[j].data)).addr);
            }
        }
        
        fprintf(fp, "end of instructions \n data: \n", i);
        for (arch_int k = 0; k < info->jobs[i]->data_count; k++) 
        {
            fprintf(fp, "#%i %i", k, ((arch_word)info->jobs[i]->data[k]));
        }
        fprintf(fp, "end of job #%i\n\n", i);      
    }
    fclose(fp);
}

void read_file(const char* path)
{
    input_file = fopen(path, "r");
    if (input_file == NULL)
    {
        send_error(parser_file_error);
    }
}

void help_add_to_job_list(job_list* list, parse_job* job)
{
    if (list->count >= list->capacity)
    {
        list->capacity += 10;
        list->jobs = realloc(list->jobs, list->capacity);
        if (list->jobs == NULL)
        {
            send_error(bad_malloc);
        }
    }

    list->jobs[list->count] = job;
    list->count++;
}

parse_job* help_create_job
    (
        job_descriptor* job_descript,
        data_descriptor* data_descript,
        arch_instr* instructions,
        arch_word* data
    )
{
    parse_job* job = malloc(sizeof(parse_job));
    if (job == NULL)
    {
        send_error(bad_malloc);
    }

    job->priority = job_descript->priority;
    job->instr_count = job_descript->instr_count;
    job->instructions = instructions;
    job->data_count = data_descript->input_buff_size
                    + data_descript->output_buff_size
                    + data_descript->temp_buff_size;
    job->data = data;
    return job;
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
