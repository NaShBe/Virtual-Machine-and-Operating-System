#include "handler.h"
#include <stdio.h>
#include <stdlib.h>


static FILE* file;

void parse_file(const char*);
void output_context(const char*);
void read_file(const char*);

void parse_file(const char* path)
{
    read_file(path);
    char current_char;
    while ((current_char = fgetc(file)) != EOF)
    {
        
    }

}

void output_context(const char* path)
{

}

void read_file(const char* path)
{
    file = fopen(path, "r");
    if (file == NULL)
    {
        send_error(bad_malloc);
    }
}

