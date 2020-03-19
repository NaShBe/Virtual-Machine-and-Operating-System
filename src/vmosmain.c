#include "vmos.h"
#include "architecture.h"
#include "handler.h"
#include "parse.h"
#include "harddrive.h"
#include "loader.h"
#include "scheduler.h"
#include "dispatcher.h"

int main(int argc, char** argv)
{
    init_error();
    job_list* main_jobs;
    arch_drive* main_drive;

    vmos_bool      is_done_executing = FALSE;
    
    if (argc > 1)
    {
        main_jobs = parse_file(argv[1]);
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
    
    init_drive(main_drive);
    write_to_disc(main_drive, main_jobs);
    get_jobs(main_drive);
    arch_core* multi_cores[2];
    multi_cores[0] = init_core_default();
    multi_cores[1] = init_core_default();
    init_scheduler(loaded_processes, multi_cores, 2);
    for (vmos_int i = 0; i < 2; i++) 
    {
        init_dispatch(multi_cores[i]);
    }

    while (is_done_executing == FALSE)
    {
        for(arch_uint i = 0; i = 2; i ++)
        {
            if(multi_cores[i]->pipeline[CORE_EXE_STEP] == halt)
            {
                swap_process(multi_cores[i]);
            }
        }
        cycle(multi_cores, 2);
    }
    /*
    
    // scheduler
    // dispatcher
    cycle(multi_cores, 2);
    */
}