#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "tcpip_adapter.h"

#include "../include/utils.h"
#include "../include/type.h"
#include "../include/handler.c"
#include "../include/config.h"

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id){
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            #if DEBUG_ENABLED
                ESP_LOGI(TAG, "[CONNECT] IP : %d.%d.%d.%d", IP2STR(&event->event_info.got_ip.ip_info.ip));
                ESP_LOGI(TAG, "[CONNECT] Netmask : %d.%d.%d.%d", IP2STR(&event->event_info.got_ip.ip_info.netmask));
                ESP_LOGI(TAG, "[CONNECT] Gateway : %d.%d.%d.%d", IP2STR(&event->event_info.got_ip.ip_info.gw));
            #endif
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void initialise_wifi(void) {
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void http_server_netconn_serve(struct netconn *conn) {
    char *buf, *payload, *start, *stop;
    struct netbuf *inbuf;
    u16_t buflen;
    err_t err;

    err = netconn_recv(conn, &inbuf);

    payload = NULL;
    if (err == ERR_OK){
        netbuf_data(inbuf, (void **)&buf, &buflen);
        start = strstr(buf, " ") + 1;
        stop = strstr(start + 1, " ");

        payload = (char *)malloc(stop - start + 1);
        memcpy(payload, start, stop - start);
            
        payload[stop - start] = '\0';
        
        switch(buf[0]) {
            case 'G' : {
                //GET
                ESP_LOGI(TAG, "[CLIENT GET] %s", payload);
                ESP_LOGI(TAG, "[CLIENT GET LENGTH] %d", strlen(payload));
                netconn_write(conn, HDR_200, sizeof(HDR_200) - 1, NETCONN_NOCOPY);
                executeGetHandlerByUrl(conn, payload);
                break;
            }
            default : {
                netconn_write(conn, HDR_501, sizeof(HDR_501) - 1, NETCONN_NOCOPY);
                break;
            }
        }
        free(payload);
    }
    netconn_close(conn);
    netbuf_delete(inbuf);
}

static void http_server(void *pvParameters) {
    const uint16_t port = (uint16_t)(
                (pvParameters != NULL) 
                ? (uint16_t)pvParameters 
                : 80);
    ESP_LOGI(TAG, "[INFO] Port Open : %d", port);
    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, port);
    netconn_listen(conn);
    do {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while (err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
    if(xEventGroupGetBits(wifi_event_group) & NEED_REBOOT_BIT){
        ESP_LOGD(TAG, "[GPIO] Reboot Sequence");
        gpio_set_level(GPIO_PINS[0], LOW);
        delay(5000);
        gpio_set_level(GPIO_PINS[0], HIGH);
        ESP_LOGD(TAG, "[GPIO] Power OFF Command");
        delay(50);
        gpio_set_level(GPIO_PINS[0], LOW);
        delay(100);
        gpio_set_level(GPIO_PINS[0], HIGH);
        ESP_LOGD(TAG, "[GPIO] Reboot Command");
        xEventGroupClearBits(wifi_event_group, NEED_REBOOT_BIT);
        ESP_LOGD(TAG, "[BIT] Clear NEED_REBOOT_BIT");
    }
}

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();
    for (int i = 0; i < sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]); i++){
        gpio_pad_select_gpio(GPIO_PINS[i]);
        gpio_set_direction(GPIO_PINS[i], GPIO_MODE_OUTPUT);
        gpio_set_level(GPIO_PINS[i], HIGH);
    }
    xTaskCreate(&http_server, "http_server", 4096, NULL, 1, NULL);
}
