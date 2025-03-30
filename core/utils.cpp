#include "utils.h"
#include <cstdlib>

std::string utils::intToStr(int a)
{
    char buf[10]{0};
    snprintf(buf, 10, "%d", a);
    return buf;
}

int utils::strToInt(const char *a)
{
    if (!a) {
        return 0;
    }
    return atoi(a);
}
