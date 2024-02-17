#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "defs.h"
#include "reader.h"

#include <mutex>
#include <string>

#define BUF_SIZE (128)
#define PIN_TXD (UART_PIN_NO_CHANGE)
#define PIN_RTS (UART_PIN_NO_CHANGE)
#define PIN_CTS (UART_PIN_NO_CHANGE)

static std::mutex last_cardid_mutex;
static RDM6300::Card_id last_cardid;

RDM6300::Card_id get_and_clear_last_cardid()
{
    std::lock_guard<std::mutex> g(last_cardid_mutex);
    const auto id = last_cardid;
    last_cardid = 0;
    return id;
}

void rfid_task(void*)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, PIN_TXD, PIN_RFID_RXD, PIN_RTS, PIN_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t data[BUF_SIZE];

    RDM6300 decoder;

    while (1)
    {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len)
        {
             for (int i = 0; i < len; ++i)
                  if (decoder.add_byte(data[i]))
                  {
                       std::lock_guard<std::mutex> g(last_cardid_mutex);
                       last_cardid = decoder.get_id();
                  }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
