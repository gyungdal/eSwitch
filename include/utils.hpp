#ifndef __UTILS_HPP__
#define __UTILS_HPP__

// x65599 hashing algorithm
const unsigned long long Hash(const char* str){
    unsigned long long hash = 0;
    while (*str){
        hash = 65599 * hash + str[0];
        str++;
    }
    return hash ^ (hash >> 16);
}

#endif