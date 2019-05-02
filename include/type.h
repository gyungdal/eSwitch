#ifndef __TYPE_H__
#define __TYPE_H__

#include "lwip/api.h"

typedef enum _gpio_status_t {
    LOW = 0,
    HIGH = 1
} gpio_status_t;

typedef struct _url_handler_t {
    char* url;
    void (*handler)(struct netconn* conn, char* url, char* payload);
} url_handler_t;

#endif