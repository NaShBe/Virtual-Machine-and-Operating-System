#include "parse.h"
#include "handler.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_MAX        50
#define HEADER_OFFSET   3

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

static FILE* input_file;
static FILE* context_file;

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
    jobs->size = 0;
    jobs->jobs = NULL;

    while (fgets(read_line, LINE_MAX, input_file) != NULL)  // we read every line
    {
        instruction_list* instr_list;                       // this will save all the instructions for the specific job being parsed
        arch_word*        data_list;                        // this will save all the data for the specific job being parsed
        arch_char* field_type;
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
            
            header_type = help_to_lower_string(header_type); // this is so capitalization does not cause errors
            // HELP_TO_LOWER_STRING NOT IMPLEMENTED YET

            if (strcmp(header_type, "job") == 0)            // If the header is `job`, fill the job descriptor with the appropriate information
            {                                               // that information will be used to add to instr_list, the list of instructions for this job
                if (job == NULL)
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

                }

                else
                {
                    send_error(parser_file_error);
                }
                
            }

            else if (strcmp(field_type, "end") == 0)    // NOT FINISHEDL if the header is `end` and both the job and data descriptor are filled out,
            {                                           // create the job and add it to the job list
                if (data == NULL || job == NULL)
                {
                    send_error(parser_file_error);
                }

                else
                {
                    
                }
            }
        }
        else if (strcmp(field_type, "0x") == 0);        // UNFINISHED: this code will write to instr_list and data_list
        {
            if (job != NULL)
            {
                for (arch_int i = 0; i < job->instr_count; i++)
                {
                    
                }
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