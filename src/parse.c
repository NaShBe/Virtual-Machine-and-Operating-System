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
#define HEADER_MAX_SIZE 5
#define FIELD_MAX_SIZE  3

#define DATA_INPUT_BUFFER_SIZE  0x14
#define DATA_OUTPUT_BUFFER_SIZE 0xC
#define DATA_TEMP_BUFFER_SIZE   0xC
#define DATA_TOTAL_SIZE DATA_TEMP_BUFFER_SIZE + DATA_INPUT_BUFFER_SIZE + DATA_OUTPUT_BUFFER_SIZE

#define NO_NULL_STRING_MAX 10
#define ASCII_OR_OFFSET    32
#define MNEUMONIC_MAX      5

static FILE* input_file = NULL;
static FILE* context_file = NULL;

job_list* parse_file(const char*);
void output_context(job_list*, const arch_char*);
void read_file(const char*);
void help_add_to_job_list(job_list* list, parse_job* job);
parse_job* help_create_job(job_descriptor* job_descript, data_descriptor* data_descript, arch_instr* instructions, arch_word* data);
arch_instr help_parse_to_instr(arch_uint data);
arch_char* help_to_lower_string(arch_char* string);


job_list* parse_file(const char* path)
{
    printf("Parsing file...\n");
    if (path != NULL)
    {
        read_file(path);
    }

    if (input_file == NULL)
    {
        send_error(parser_file_error);
    }

    arch_char*          read_line = malloc(sizeof(arch_char) * LINE_MAX);
    job_descriptor*     job = NULL;
    data_descriptor*    data = NULL;
    
    job_list* jobs = malloc(sizeof(job_list));
    if (jobs == NULL)
    {
        send_error(bad_malloc);
    }
    jobs->capacity = PARSE_INIT_CAPACITY;
    jobs->count = 0;
    jobs->jobs = malloc(sizeof(parse_job*)*jobs->capacity);

    instruction_list*   instr_list = NULL;                       
    arch_word*          data_list = NULL;
    arch_int            line_number = -1; 

    while (fgets(read_line, LINE_MAX, input_file) != NULL)
    {
        line_number++;
        arch_char*        field_type = malloc(sizeof(arch_char) * FIELD_MAX_SIZE);
        strlcpy(field_type, read_line, FIELD_MAX_SIZE);        
        if (strcmp(field_type, "//") == 0)                  
        {
            arch_char* header_type;
            arch_char* field1;
            arch_char* field2;
            arch_char* field3;

            strtok(read_line, " ");

            header_type = strtok(NULL, " ");
            if (header_type == NULL)
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
                    field3 = strtok(NULL, "\n");
                    if (field3 == NULL)
                    {
                        send_error(parser_file_error);
                    }
                    
                    sscanf(field1, "%x", &job->job_num);
                    sscanf(field2, "%x", &job->instr_count);
                    sscanf(field3, "%x", &job->priority);
                }
                else
                {
                    send_error(parser_file_error);
                }
            }

            else if (strcmp(header_type, "data") == 0)
            {
                if (data == NULL && job != NULL)
                {
                    data = malloc(sizeof(data_descriptor));
                    if (data == NULL)
                    {
                        send_error(bad_malloc);
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
                    field3 = strtok(NULL, "\n");
                    if (field3 == NULL)
                    {
                        send_error(parser_file_error);
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

            else if (strcmp(header_type, "end") == 0 || strcmp(header_type, "end\n") == 0 )
            {
                if (data == NULL || job == NULL)
                {
                    send_error(parser_file_error);
                }
                else
                {
                    parse_job* new_job = help_create_job(job, data, instr_list->instructions, data_list);
                    help_add_to_job_list(jobs, new_job);
                    job = NULL;
                    data = NULL;
                    instr_list = NULL;
                    data_list = NULL;
                }
            }
        }
        else if (strcmp(field_type, "0x") == 0)
        {
            if (job != NULL && data == NULL)
            {
                if (instr_list == NULL)
                {
                    instr_list = malloc(sizeof(instruction_list));
                    instr_list->instructions = malloc(sizeof(arch_instr) * job->instr_count);
                    instr_list->capacity = job->instr_count;
                    if (instr_list == NULL)
                    {
                        send_error(bad_malloc);
                    }                 
                }
                for (arch_int i = 0; i < job->instr_count; i++)
                {
                    instr_list->size++;
                    arch_word instr_word;
                    sscanf(read_line, "%x%*c", &instr_word);
                    if (i < job->instr_count -1)
                    {
                        line_number++;
                        if (fgets(read_line, LINE_MAX, input_file) == NULL)
                        {
                            send_error(parser_file_error);
                        }
                    }
                    
                    instr_list->instructions[i].format = (instr_word & BITFIELD_FORMAT) >> BITOFFSET_FORMAT;
                    instr_list->instructions[i].opcode = (instr_word & BITFIELD_OPCODE) >> BITOFFSET_OPCODE;
                    arch_uint r1;
                    arch_uint r2;
                    arch_uint r3;
                    arch_uint data;
                    switch(instr_list->instructions[i].format)
                    {
                        case FORMAT_AIF:
                            r1 = (instr_word & BITFIELD_OPRAND1) >> BITOFFSET_OPRAND1;
                            r2 = (instr_word & BITFIELD_OPRAND2) >> BITOFFSET_OPRAND2;
                            r3 = (instr_word & BITFIELD_OPRAND3) >> BITOFFSET_OPRAND3;
                            data = 0;
                            instr_list->instructions[i].data = (arith_data){.src1 = r1, .src2 = r2, .dest = r3, .zr = data}.int_rep;
                            break;
                        case FORMAT_CBIF:
                            r1 = (instr_word & BITFIELD_OPRAND1) >> BITOFFSET_OPRAND1;
                            r2 = (instr_word & BITFIELD_OPRAND2) >> BITOFFSET_OPRAND2;
                            data = instr_word & BITFIELD_ADDRESS;
                            instr_list->instructions[i].data = (cond_imm_data){.breg = r2, .dreg = r1, .addr = data}.int_rep;
                            break;
                        case FORMAT_UJF:
                            instr_list->instructions[i].data = instr_word & !(BITFIELD_FORMAT | BITFIELD_OPCODE);
                            break;
                        case FORMAT_IOIF:
                            r1 = (instr_word & BITFIELD_OPRAND1) >> BITOFFSET_OPRAND1;
                            r2 = (instr_word & BITFIELD_OPRAND2) >> BITOFFSET_OPRAND2;
                            data = instr_word & BITFIELD_ADDRESS;
                            instr_list->instructions[i].data = (inp_out_data){.reg1 = r1, .reg2 = r2, .addr = data}.int_rep;
                            break;
                        default:
                            send_error(parser_file_error);
                    }
                    printf("%i| instr_word: %x, int_rep: %x          format: %x, opcode: %x\n", line_number, instr_word, instr_list->instructions[i].int_rep, instr_list->instructions[i].format, instr_list->instructions[i].opcode); 
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
                    sscanf(read_line, "%x", &data_list[i]);
                    if (i < DATA_TOTAL_SIZE -1)
                    {
                        line_number++;
                        if (fgets(read_line, LINE_MAX, input_file) == NULL)
                        {
                            send_error(parser_file_error);
                        }
                    }
                }
            }
            else
            {
                send_error(parser_file_error);
            }
        }
    }
    printf("\nParsing is done... saving data onto harddrive.\n");
    fclose(input_file);
    return jobs;
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
        fprintf(fp, "// JOB: %i (0x%x) Priority: %i\n", i+1, i+1, info->jobs[i]->priority);
        for (arch_int j = 0; j < info->jobs[i]->instr_count; j++)
        {
            arch_char* opcode_mneum;
            arch_char* description;
            switch (info->jobs[i]->instructions[j].opcode)
            {
                case RD:
                    opcode_mneum = "RD";
                    description = "Reading";
                    break;
                case WR:
                    opcode_mneum = "WR";
                    description = "Writing";
                    break;
                case ST:
                    opcode_mneum = "ST";
                    description = "Storing content of register";
                    break;
                case LW:
                    opcode_mneum = "LW";
                    description = "Loading content of register";
                    break;
                case MOV:
                    opcode_mneum = "MOV";
                    description = "Moving content of register";
                    break;
                case ADD:
                    opcode_mneum = "ADD";
                    description = "Adding content of register";
                    break;
                case SUB:
                    opcode_mneum = "SUB";
                    description = "Subtracting content of register";
                    break;
                case MUL:
                    opcode_mneum = "MUL";
                    description = "Multiplying content of register";
                    break;
                case DIV:
                    opcode_mneum = "DIV";
                    description = "Dividing content of register";
                    break;
                case AND:
                    opcode_mneum = "AND";
                    description = "Perform logical AND between content of register";
                    break;
                case OR:
                    opcode_mneum = "OR";
                    description = "Perform logical OR between content of register";
                    break;
                case MOVI:
                    opcode_mneum = "MOVI";
                    description = "Move immediate value";
                    break;
                case ADDI:
                    opcode_mneum = "ADDI";
                    description = "Add immediate value";
                    break;
                case MULI:
                    opcode_mneum = "MULI";
                    description = "Multiply immediate value";
                    break;
                case DIVI:
                    opcode_mneum = "DIVI";
                    description = "Divide content of register";
                    break;
                case LDI:
                    opcode_mneum = "LDI";
                    description = "Load immediate value";
                    break;
                case SLT:
                    opcode_mneum = "SLT";
                    description = "Set content of register";
                    break;
                case SLTI:
                    opcode_mneum = "SLTI";
                    description = "Set content of register";
                    break;
                case HTL:
                    opcode_mneum = "HTL";
                    description = "Halt program, logical end";
                    break;
                case NOP:
                    opcode_mneum = "NOP";
                    description = "No operation";
                    break;
                case JMP:
                    opcode_mneum = "JMP";
                    description = "Jump to address";
                    break;
                case BEQ:
                    opcode_mneum = "BEQ";
                    description = "Branch to";
                    break;
                case BNE:
                    opcode_mneum = "BNE";
                    description = "Branch to";
                    break;
                case BEZ:
                    opcode_mneum = "BEZ";
                    description = "Branch to";
                    break;
                case BNZ:
                    opcode_mneum = "BNZ";
                    description = "Branch to";
                    break;
                case BGZ:
                    opcode_mneum = "BGZ";
                    description = "Branch to";
                    break;
                case BLZ:
                    opcode_mneum = "BLZ";
                    description = "Branch to";
                    break;
                case SWE:
                    opcode_mneum = "SWE";
                    description = "Software Interrupt";
                    break;
            }
            
            arch_uint r1;
            arch_uint r2;
            arch_uint r3;
            arch_uint data;
            switch(info->jobs[i]->instructions[j].format)
            {
                case FORMAT_AIF:
                    r1 = ((arith_data)(info->jobs[i]->instructions[j].data)).src1;
                    r2 = ((arith_data)(info->jobs[i]->instructions[j].data)).src2;
                    r3 = ((arith_data)(info->jobs[i]->instructions[j].data)).dest;
                    fprintf(fp, "%i: %s Rs%i Rs%i Rd%i\t\t// %s\n", j+1, opcode_mneum, r1, r2, r3, description);
                    break;
                case FORMAT_CBIF:
                    r1 = ((cond_imm_data)(info->jobs[i]->instructions[j].data)).breg;
                    r2 = ((cond_imm_data)(info->jobs[i]->instructions[j].data)).dreg;
                    data = ((cond_imm_data)(info->jobs[i]->instructions[j].data)).addr;
                    fprintf(fp, "%i: %s Rb%i Rd%i 0x%x\t\t// %s\n", j+1, opcode_mneum, r1, r2, data, description);
                    break;
                case FORMAT_UJF:
                    data = info->jobs[i]->instructions[j].data;
                    fprintf(fp, "%i: %s 0x%x\t\t// %s\n", j+1, opcode_mneum, data, description);
                    break;
                case FORMAT_IOIF:
                    r1 = ((inp_out_data)(info->jobs[i]->instructions[j].data)).reg1;
                    r2 = ((inp_out_data)(info->jobs[i]->instructions[j].data)).reg2;
                    data = ((inp_out_data)(info->jobs[i]->instructions[j].data)).addr;
                    fprintf(fp, "%i: %s Rs%i Rs%i 0x%x\t\t// %s\n", j+1, opcode_mneum, r1, r2, data, description);
                    break;
            }
        }
        
        fprintf(fp, "// DATA for Job %i:\n", i+1);
        for (arch_int k = 0; k < info->jobs[i]->data_count; k++) 
        {
            fprintf(fp, "%i: %i\n", k+1, ((arch_word)info->jobs[i]->data[k]));
        }
        fprintf(fp, "// END of Job #%i\n", i+1);      
    }
    fclose(fp);
    printf("(contextualized parsing data written into context.txt)\n\n");
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
    arch_char* c = string;
    arch_uint iter = 0;
    while (c != '\0' && iter < NO_NULL_STRING_MAX)
    {
        *c = tolower(*c);
        c++;
        iter++;
    }

    return string;
}
