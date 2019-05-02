#ifndef __TYPE_H__
#define __TYPE_H__

#include "lwip/api.h"

const uint32_t HIGH = 1;
const uint32_t LOW = 0;

typedef struct _url_handler_t {
    char* url;
    void (*handler)(struct netconn* conn, char* url, char* payload);
} url_handler_t;

#endif