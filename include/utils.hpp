#ifndef __UTILS_HPP__
#define __UTILS_HPP__

// x65599 hashing algorithm
unsigned long long Hash(const char* str){
    return str[0] ? 0xEDB8832Full * Hash(&str[1]) + str[0] : 8603;
}

#endif