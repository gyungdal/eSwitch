#ifndef __HANDLER_CPP__
#define __HANDLER_CPP__

#include <string.h>
#include "./type.hpp"

static void get_root_handler(struct netconn* conn, char* url, char* payload){
    const char* body = "HELLO WORLD!\0";
    netconn_write(conn, body, strlen(body) - 1, NETCONN_NOCOPY);
}

url_handler_t get_handlers[] = {
    { 
        .url = "/",
        .handler = get_root_handler
    }
};

err_t executeHandlerByUrl(url_handler_t* handlers, struct netconn* conn, char* payload){
    char *start, *middle, *url;
    start = payload;
    if(strstr(payload, "?") != NULL){
        //경로 뒤에 값이 있는 경우
        middle = strstr(payload, "?");
        url = (char*)malloc(middle - start + 1);
        memcpy(url, start, middle - start);
        url[middle - start] = '\0';
    }else{
        //경로만 있는 경우
        size_t len = strlen(payload) + 1;
        url = (char*)malloc(len * sizeof(char));
        memcpy(url, payload, len);
    }
    for(size_t i = 0;i<sizeof(handlers) / sizeof(handlers[0]);i++){
        if(strcmp(url, handlers[i].url) == 0){
            handlers[i].handler(conn, url, NULL);
        }
    }
    free(url);
    return ESP_ERR_NOT_FOUND;
}
 
#endif