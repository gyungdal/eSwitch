#include "utils.h"

// x65599 hashing algorithm
unsigned int Hash(const char *str)
{
    unsigned int hash = 0;
    while (*str)
    {
        hash = 65599 * hash + str[0];
        str++;
    }
    return hash ^ (hash >> 16);
}