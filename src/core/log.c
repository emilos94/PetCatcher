#include "log.h"

int log_msg(char* msg,...) {
    char buf[512];

    va_list args;
    int return_value;

    va_start(args, msg);
    return_value = vsnprintf(buf, 511, msg, args);
    va_end(args);

    printf(buf);
    return return_value;
}
