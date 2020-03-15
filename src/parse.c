#include "parse.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>


static FILE* input_file;
static FILE* context_file;

parse_jobs parse_file(const char*);
void output_context(parse_jobs*);
void read_file(const char*);

parse_jobs parse_file(const char* path)
{
    read_file(path);
    char current_char;

    while ((current_char = fgetc(input_file)) != EOF)
    {
        if (current_char != '\n')
        {

        }
    }

    fclose(input_file);
}

void output_context(parse_jobs* info)
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

void help_add_to_jobs(parse_data* job)
{
    
}