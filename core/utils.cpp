#include "utils.h"

std::string utils::intToStr(int a)
{
    char buf[10]{0};
    itoa(a, buf, 10);
    return buf;
}

int utils::strToInt(const char *a) { return atoi(a); }
