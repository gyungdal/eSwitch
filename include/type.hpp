#ifndef __TYPE_HPP__
#define __TYPE_HPP__

typedef enum _gpio_status_t {
    LOW = 0,
    HIGH = 1
} gpio_status_t;

typedef struct _url_handler_t {
    char* url;
    void (*handler)(struct netconn* conn);
} url_handler_t;

#endif