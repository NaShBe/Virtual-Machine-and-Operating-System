#ifndef NBERAKI_HANDLER_H
#define NBERAKI_HANDLER_H

#define send_error(x) signal_abort(x, __FILE__, __LINE__)

typedef enum
{
    undefined = 0,
    ram_filled,
    ram_outbounds,
    reg_invalid
} error_type_enum;

extern error_type_enum error_type;

extern inline void init_error();
extern void signal_abort(error_type_enum, char*, int);  /* Don't call this directly, use send_error(<error_type>) or call abort */

#endif