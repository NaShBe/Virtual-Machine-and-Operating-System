#include "architecture.h"
#include "handler.h"
#include "parse.h"

int main(int argc, char** argv)
{

    job_list* test_jobs = parse_file(argv[1]);

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