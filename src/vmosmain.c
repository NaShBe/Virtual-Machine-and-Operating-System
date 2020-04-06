#include "vmos.h"
#include "architecture.h"
#include "instructions.h"
#include "handler.h"
#include "parse.h"
#include "harddrive.h"
#include "loader.h"
#include "scheduler.h"
#include "dispatcher.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

job_list*   main_jobs = NULL;
arch_drive* main_drive = NULL;

vmos_bool   is_done_executing = FALSE;

int main(int argc, char** argv)
{
    init_error();
    clock_t parsing_time;
    clock_t harddrive_time;
    clock_t loading_time;
    clock_t scheduling_time;
    clock_t dispatching_time;
    
    if (argc > 1)
    {
        parsing_time = clock();
        main_jobs = parse_file(argv[1]);
        parsing_time = clock() - parsing_time;

    }
    else
    {
        send_error(file_no_input);
    }
    if (argc > 2)
    {
        output_context(main_jobs, argv[2]);
    }
    else
    {
        output_context(main_jobs, "context.txt");
    }

    harddrive_time = clock();
    main_drive = malloc(sizeof(drive_file));
    init_drive(main_drive);
    write_to_disc(main_drive, main_jobs);
    harddrive_time = clock() - harddrive_time;

    loading_time = clock();
    get_jobs(main_drive);
    output_loader("context.txt");
    load_jobs();
    loading_time = clock() - loading_time;

    arch_core** multi_cores;
    multi_cores = malloc(sizeof(arch_core*) * CORE_COUNT);
    for (vmos_int i = 0; i < CORE_COUNT; i ++)
    {
        multi_cores[i] = malloc(sizeof(arch_core));
        multi_cores[i] = init_core_default();
    }
    init_scheduler(multi_cores, CORE_COUNT);
    init_dispatcher(multi_cores, CORE_COUNT);
    output_loader("context.txt");
    cycle(multi_cores, CORE_COUNT);

    while (is_done_executing == FALSE)
    {
        scheduling_time = clock();
        schedule_tasks();
        scheduling_time = clock() - scheduling_time;

        dispatching_time = clock();
        dispatch_cores();
        dispatching_time = clock() - dispatching_time;
        load_jobs();
        free_jobs();

        printf("Parse time: %lu\nStorage time: %lu\nLoading time: %lu\nScheduling time: %lu\nDispatching time: %lu\n\n", parsing_time, harddrive_time, loading_time, scheduling_time, dispatching_time);
    }
}