#include "main.h"

static TaskHandle_t xIrTask;

void app_main() {}

/**
 * Initialize wifi by the following scheme.
 *  - Init hardware and software, attempt connect using saved SSID
 *  - If connect fails, start smart config task
 * 
 */
void init_wifi()
{
    EasyWifi::init_hardware();
    EasyWifi::init_software();
    EasyWifi::connect();

    const uint32_t wifiConnTimeoutSec = 20;
    EasyWifi::wait_for_wifi(wifiConnTimeoutSec);

    if (!EasyWifi::is_connected()) {
        uint32_t sc_timeout = SC_TIMEOUT_S;
        SmartConfig::sc_start(&sc_timeout);
    }

    while (true) {
    }
}

/**
 * Initialize the IR code send hardware
 * 
 */
void init_remote()
{
    ESP_LOGI("", "Running init_remote()");

    /* Configure IR characteristics */
    config.channel = RMT_CHANNEL_0;
    config.gpio_num = (gpio_num_t)RMT_TX_GPIO_NUM;
    config.tx_config.carrier_duty_percent = RMT_CARRIER_DUTY_PERCENT;
    config.tx_config.carrier_en = true;
    config.tx_config.loop_en = true;
    config.tx_config.carrier_freq_hz = RMT_CARRIER_FREQ_HZ;
    rmt_config(&config);
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, RMT_NO_RX_BUFFER, 0));

    /* Configure timings */
    rmt_item32_t low;
    low.duration0 = 10;

    /* Init task used to handle code sending */
    xTaskCreate(send_code_task, "send_code", 2048, NULL, 5, &xIrTask);

    ESP_LOGI("", "Remote initialized");
}

/**
 * Initialize the led used as an indicator
 * 
 */
void init_led()
{
    ESP_LOGI("led", "Initializing LED");

    ledc_channel_config_t led_config;
    led_config.channel = LEDC_CHANNEL_0;
    led_config.gpio_num = LED_GPIO;
    ledc_channel_config(&led_config);

    ESP_LOGI("led", "LED initialized");
}

