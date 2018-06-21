#include "main.h"
 
void app_main() {}

void init_wifi() {
    EasyWifi::init_hardware();
    EasyWifi::init_software();
    EasyWifi::connect();

    const uint32_t wifi_wait_time_s = 20;
    EasyWifi::wait_for_wifi(wifi_wait_time_s);

    if(!EasyWifi::is_connected()) {
        uint32_t sc_timeout = SC_TIMEOUT_S;
        SmartConfig::sc_start(&sc_timeout);
    }

    while(true) {

    }
}

void init_remote() {
    ESP_LOGI("", "Running init_remote()");

    /* Configure IR characteristics */
    config.channel = RMT_CHANNEL_0;
    config.gpio_num = (gpio_num_t) RMT_TX_GPIO_NUM;
    config.tx_config.carrier_duty_percent = RMT_CARRIER_DUTY_PERCENT;
    config.tx_config.carrier_en = true;
    config.tx_config.loop_en = true;
    config.tx_config.carrier_freq_hz = RMT_CARRIER_FREQ_HZ;
    rmt_config(&config);
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, RMT_NO_RX_BUFFER, 0));

    /* Configure timings */
    rmt_item32_t low;
    low.duration0 = 10;

    ESP_LOGI("", "Remote initialized");
}

void init_led() {
    ESP_LOGI("led", "Initializing LED");

    ledc_channel_config_t led_config;
    led_config.channel = LEDC_CHANNEL_0;
    led_config.gpio_num = LED_GPIO;
    ledc_channel_config(&led_config);
    
    ESP_LOGI("led", "LED initialized");
}

void send_code(rmt_item32_t *items, uint16_t num_items) {

    /* Load the code into buffer */
    rmt_fill_tx_items(config.channel, items, num_items, RMT_NO_MEM_OFFSET);
    xTaskCreate(send_code_task, "send_code", 2048, nullptr, 5, nullptr);
}

void send_code_task(void *parm) {
    ESP_LOGI("", "Starting code send");

    /* Turn on indicator LED and start code send */
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 100);
    ESP_ERROR_CHECK(rmt_tx_start(config.channel, true));

    /* Delay for a few repeats to ensure code is received */
    vTaskDelay(Delay::ms_to_ticks(RMT_TX_WINDOW_MS));

    /* Stop code send, turn LED off, end task */
    ESP_ERROR_CHECK(rmt_tx_stop(RMT_CHANNEL_0));
    ESP_LOGI("", "Code send completed");
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    vTaskDelete(NULL);
}
