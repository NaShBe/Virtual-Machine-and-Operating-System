#include "architecture.h"
#include "handler.h"
#include "parse.h"

int main(int argc, char** argv)
{
    init_error();
    job_list* main_jobs;
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
    

    

    /*
    init_error();
    arch_core* multi_cores[2];
    multi_cores[0] = init_core_default();
    multi_cores[1] = init_core_default();
    cycle(multi_cores, 2);
    */

    // loader
    // scheduler
    // dispatcher
}