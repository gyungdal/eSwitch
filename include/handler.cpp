#ifndef __HANDLER_CPP__
#define __HANDLER_CPP__

#include <string.h>
#include "./type.hpp"

static void get_root_handler(struct netconn* conn){
    const char* body = "HELLO WORLD!\0";
    netconn_write(conn, body, sizeof(body) - 1, NETCONN_NOCOPY);
}

url_handler_t get_handlers[] = {
    { 
        .url = "/",
        .handler = get_root_handler
    }
};

url_handler_t* findByKey(url_handler_t* handlers, char* key){
    for(size_t i = 0;i<sizeof(handlers) / sizeof(handlers[0]);i++){
        if(!strncmp(key, handlers[i].url, sizeof(key))){
            return &handlers[i];
        }
    }
    return nullptr;
}
 
#endif