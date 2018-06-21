#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "gpio.h"
#include "ledc.h"
#include "soc/rmt_reg.h"
#include "SmartConfig/EasyWifi.h"
#include "SmartConfig/SmartConfig.h"
#include "remote.h"

#define SC_TIMEOUT_S    300
#define LED_GPIO        GPIO_NUM_4

rmt_config_t config;

extern "C" {
void app_main();
}

void init_wifi();

void init_remote();

void init_led();

void led_pulse_indicator();

void send_code();

void send_code_task(void *parm);