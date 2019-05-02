#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "esp_system.h"

#define HDR_200 "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n"
#define HDR_501 "HTTP/1.1 501 Not Implemented\r\nContent-type: text/html\r\n\r\n"

#define WIFI_SSID "Gyeongsik's Wi-Fi Network"
#define WIFI_PASS "gyungdal"


const int CONNECTED_BIT = BIT0;
const int NEED_BOOT_BIT = BIT1;
const gpio_num_t GPIO_PINS[] = {GPIO_NUM_21, GPIO_NUM_22};
static const char *TAG = "eSwitch";

#endif