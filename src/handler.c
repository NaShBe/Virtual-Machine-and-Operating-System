#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "handler.h"

#define ASCII_OFFSET 48
#define HANDLE_NO_FILE "Unspecified file"
#define HANDLE_NO_LINE "Unspecified line"

#define INTERN_ERROR internal_error()     /* error within handler.c, exit */
#define INTERN_EVAL error_intern_line = __LINE__           /* predicted line of internal error */

error_type_enum error_type = undefined;
static char* error_filename = NULL;
static char* error_line = NULL;
static int error_intern_line = 0;

inline void init_error();
void signal_abort(error_type_enum, char*, int);
static void abort_handle();
static char* help_errorln_itoa(int);
static void internal_error();

inline void init_error()
{
    signal(SIGABRT, abort_handle);
}

void signal_abort(error_type_enum error, char* filename, int fileline)
{
    error_type = error;
    error_filename = filename;
    error_line = help_errorln_itoa(fileline);
    abort_handle();
}

static void abort_handle()
{
    if (error_filename == NULL)
    {
        error_filename = malloc(sizeof(HANDLE_NO_FILE));  INTERN_EVAL;
        if (error_filename != NULL) strncpy(error_filename, HANDLE_NO_FILE, sizeof(HANDLE_NO_FILE));
        else INTERN_ERROR;
    }
    if (error_line == NULL)
    {
        error_line = malloc(sizeof(HANDLE_NO_LINE)); INTERN_EVAL;
        if(error_line != NULL) strncpy(error_line, HANDLE_NO_LINE, sizeof(HANDLE_NO_LINE));
        else INTERN_ERROR;
    }

    switch(error_type)
    {
        case bad_malloc:
            fprintf(stderr, "important data memory not allocated, aborting...\n%s: %s", error_filename, error_line);
            break;
        case core_pipeline_corruption:
            fprintf(stderr, "Pipeline corrupted, aborting...\n%s: %s", error_filename, error_line);
            break;
        case ram_filled:
            fprintf(stderr, "RAM filled, aborting...\n%s: %s", error_filename, error_line);
            break;
        case ram_outbounds:
            fprintf(stderr, "address request out of bounds of RAM, aborting...\n%s: %s", error_filename, error_line);
            break;
        case reg_invalid:
            fprintf(stderr, "Invalid register input/register not available, aborting...\n%s: %s", error_filename, error_line);
            break;
        case disc_not_attached:
            fprintf(stderr, "No storage device attached, aborting...\n%s: %s", error_filename, error_line);
            break;
        case disc_corruption:
            fprintf(stderr, "Disc data corrupted and cannot be read, aborting...\n%s: %s", error_filename, error_line);
            break;
        case parser_file_error:
            fprintf(stderr, "VMOS could not read the input file, aborting...\n%s: %s", error_filename, error_line);
            break;
        case file_no_input:
            fprintf(stderr, "No input detected. Please provide an input file before running VMOS.");
            break;
        default:
            fprintf(stderr, "Unspecified error, aborting....\n%s: %s", error_filename, error_line);
    }
    exit(EXIT_FAILURE);
}


static char* help_errorln_itoa(int line)
{
    if (line < 0) return NULL;

    if (line == 0)
    {
        char* zr_rt = malloc(sizeof("0")); INTERN_EVAL;
        if (zr_rt == NULL) INTERN_ERROR;
        strncpy(zr_rt, "0", sizeof("0"));
    }

    char* lnstring = NULL;
    int digit = 1;
    int size = 0;
    while (line%digit != line)
    {
        digit*=10;
        size++;
    }

    lnstring = malloc(size * sizeof(char)); INTERN_EVAL;
    if (lnstring == NULL) INTERN_ERROR;

    for (int i = 0; digit >= 10; i++)
    {
        digit/=10;
        lnstring[i] = (line/digit) + ASCII_OFFSET;
        line -= (line/digit) * digit;
    }

    return lnstring;
}

static void internal_error()
{
    fprintf(stderr, "Internal error at line: %i", error_intern_line);
    exit(EXIT_FAILURE);
}
