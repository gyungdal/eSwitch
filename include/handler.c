#ifndef __HANDLER_C__
#define __HANDLER_C__

#include <string.h>
#include <lwip/err.h>

#include "./type.h"
#include "./config.h"

static void get_root_handler(struct netconn* conn, char* url, char* payload){
    const char* body = "HELLO WORLD!\0";
    netconn_write(conn, body, strlen(body) - 1, NETCONN_NOCOPY);
}

static void reboot_request_handler(struct netconn* conn, char* url, char* payload){
    const char* body = "CONFIRM!\0";
    netconn_write(conn, body, strlen(body) - 1, NETCONN_NOCOPY);
    xEventGroupSetBits(wifi_event_group, NEED_REBOOT_BIT);
}

static url_handler_t get_handlers[] = {
    { 
        .url = "/",
        .handler = get_root_handler
    },
    {
        .url = "/reboot",
        .handler = reboot_request_handler
    }
};

err_t executeGetHandlerByUrl(struct netconn* conn, char* payload){
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
    for(size_t i = 0;i < sizeof(get_handlers) / sizeof(get_handlers[0]);i++){
        if(!strcmp(url, get_handlers[i].url)){
            get_handlers[i].handler(conn, url, NULL);
            return (err_t)ESP_OK;
        }
    }
    free(url);
    return (err_t)ESP_ERR_NOT_FOUND;
}
 
#endif