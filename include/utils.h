#ifndef __UTILS_H__
#define __UTILS_H__

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

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