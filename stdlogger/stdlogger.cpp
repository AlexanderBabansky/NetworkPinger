#include "NMLogger.h"
#include <cstdio>
#include <cstdarg>

void RE_LOG_ERROR(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
