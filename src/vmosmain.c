#include "architecture.h"
#include "handler.h"

int main(int argc, char** args)
{
    init_error();
    arch_core* multi_cores[2];
    multi_cores[0] = init_core_default();
    multi_cores[1] = init_core_defaults();
    cycle(multi_cores, 2);

    // loader
    // scheduler
    // dispatcher
}