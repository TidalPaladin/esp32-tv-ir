#include "remote.h"

static TaskHandle_t ir_task_handle;
static rmt_config_t ir_config;
static const char* irTAG = "IR";

esp_err_t ir_set_config(rmt_config_t* pNewConfig)
{
    if (pNewConfig == NULL) {
        ESP_LOGE(irTAG, "Function ir_set_config received null pointer");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy((void*)&ir_config, (void*)pNewConfig, sizeof(rmt_config_t));
    return ESP_OK;
}

esp_err_t ir_initiate_tx(rmt_item32_t* pCodeItems, uint16_t ulNumItems)
{
    if (pCodeItems == NULL) {
        ESP_LOGE(irTAG, "Function ir_inititae_tx received null pointer");
        return ESP_ERR_INVALID_ARG;
    }

    if (ulNumItems == 0) {
        ESP_LOGW(irTAG, "Function ir_initiate_tx got ulNumItems == 0");
    }
    /* Entire code is loaded into buffer before send beings */
    rmt_fill_tx_items(ir_config.channel, pCodeItems, ulNumItems, RMT_NO_MEM_OFFSET);
    xTaskNotifyGive(ir_task_handle);
}

static void ir_task(void* parm)
{
    ESP_LOGI("", "Starting code send task");
    for (;;) {

        /* Wait for notofication to begin code send */
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        /* Turn on indicator LED and start code send */
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 100);
        rmt_tx_start(config.channel, true);

        /* Delay for a few repeats to ensure code is received */
        vTaskDelay(pdMS_TO_TICKS(RMT_TX_WINDOW_MS));

        /* Stop code send, turn LED off, end task */
        ESP_ERROR_CHECK(rmt_tx_stop(RMT_CHANNEL_0));
        ESP_LOGI("", "Code send completed");
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    }
}