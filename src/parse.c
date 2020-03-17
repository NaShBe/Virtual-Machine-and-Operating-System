#include "parse.h"
#include "handler.h"
#include "instructions.h"
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
void output_context(job_list*);
void read_file(const char*);

job_list* parse_file(const char* path)      // takes a path to start reading from
{
    if (path != NULL)                       // this is in case we don't use read_file() first
    {
        read_file(path);
    }

    if (input_file == NULL)
    {
        send_error(parser_file_error);
    }

    arch_char*          read_line;          // we read the input file line by line, we parse it in a while loop
    job_descriptor*     job = NULL;         // a description of the job
    data_descriptor*    data = NULL;        // a description of the data attached to the job
    
    job_list* jobs = malloc(sizeof(job_list));  // this is the list of jobs parsed from the input file
    if (jobs == NULL)
    {
        send_error(bad_malloc);
    }
    jobs->capacity = PARSE_INIT_CAPACITY;       // initializing the list of jobs
    jobs->count = 0;
    jobs->jobs = NULL;

    while (fgets(read_line, LINE_MAX, input_file) != NULL)  // we read every line
    {
        instruction_list* instr_list = NULL;                       // this will save all the instructions for the specific job being parsed
        arch_word*        data_list = NULL;                        // this will save all the data for the specific job being parsed
        arch_char* field_type = NULL;
        strlcpy(field_type, read_line, SIZE_FORMAT);        // we check the field type "//" or "0x" to determine if its a header or actual data
        if (strcmp(field_type, "//") == 0)                  // this if statement holds the code for dealing with headers "job, data, and end"
        {
            arch_char* header_type;                         // `job`, `data`, or `end`
            // the meaning of these fields depends on the header type, `end` will ignore these
            // for `job`: field1 = job#; field2 = instruction count; field3 = priority
            // for `data`: field1 input buffer size; field2 = output buffer size; field3 = temp buffer size
            arch_char* field1;
            arch_char* field2;
            arch_char* field3;                              

            /* we used strtok to get each string separated by " " (space)
               for example: "JOB 3 12 2" will be parsed into their respective variables:
                - header_type = "JOB"
                - field1 = "3"
                - field2 = "12"
                - field3 = "2"
            */

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

            if (strcmp(header_type, "job") == 0)            // If the header is `job`, fill the job descriptor with the appropriate information
            {                                               // that information will be used to add to instr_list, the list of instructions for this job
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

            else if (strcmp(field_type, "data") == 0)   // NOT IMPLEMENTED: if the header is `data`, the data descriptor is filled out
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
        else if (strcmp(field_type, "0x") == 0);        // UNFINISHED: this code will write to instr_list and data_list
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
                    sscanf(read_line + SIZE_FORMAT, "%i", &instr_list[i]);
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
                    sscanf(read_line + SIZE_FORMAT, "%i", &data_list[i]);
                }
            }
            else
            {
                send_error(parser_file_error);
            }
        }
    }
    fclose(input_file);     // close the input file, we are done parsing.
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