/**
 * Consants for esp32 IR remote operation
 * 
 */

#ifndef __TV_IR_REMOTE_H__
#define __TV_IR_REMOTE_H__

#include "driver/periph_ctrl.h"
#include "driver/rmt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "gpio.h"
#include "ledc.h"
#include "remote.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#include "soc/rmt_reg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* My defines */
#define RMT_TX_GPIO_NUM ((gpio_num_t)GPIO_NUM_2)
#define RMT_CARRIER_FREQ_HZ 48000
#define RMT_CARRIER_DUTY_PERCENT 50
#define RMT_NO_RX_BUFFER 0
#define RMT_NO_MEM_OFFSET 0
#define RMT_TX_WINDOW_MS 1000

#define RMT_TX_CHANNEL 1 /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM 18 /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL 0 /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM 19 /*!< GPIO number for receiver */
#define RMT_CLK_DIV 100 /*!< RMT counter clock divider */
#define RMT_TICK_10_US (80000000 / RMT_CLK_DIV / 100000) /*!< RMT counter value for 10 us.(Source clock is APB clock) */

#define NEC_HEADER_HIGH_US 9000 /*!< NEC protocol header: positive 9ms */
#define NEC_HEADER_LOW_US 4500 /*!< NEC protocol header: negative 4.5ms*/
#define NEC_BIT_ONE_HIGH_US 560 /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US (2250 - NEC_BIT_ONE_HIGH_US) /*!< NEC protocol data bit 1: negative 1.69ms */
#define NEC_BIT_ZERO_HIGH_US 560 /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US (1120 - NEC_BIT_ZERO_HIGH_US) /*!< NEC protocol data bit 0: negative 0.56ms */
#define NEC_BIT_END 560 /*!< NEC protocol end: positive 0.56ms */
#define NEC_BIT_MARGIN 20 /*!< NEC parse margin time */

#define NEC_ITEM_DURATION(d) ((d & 0x7fff) * 10 / RMT_TICK_10_US) /*!< Parse duration time from memory register value */
#define NEC_DATA_ITEM_NUM 34 /*!< NEC code item number: header + 32bit data + end */
#define RMT_TX_DATA_NUM 100 /*!< NEC tx test data number */
#define rmt_item32_tIMEOUT_US 9500 /*!< RMT receiver timeout value(us) */

void ir_initiate_tx(rmt_item32_t* items, uint16_t num_items);

void ir_task(void* parm);

#endif