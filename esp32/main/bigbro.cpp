#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hagl_hal.h>
#include <hagl.h>

#include "console.h"
#include "defs.h"
#include "reader.h"

auto display = hagl_init();

extern "C"
void app_main()
{
    printf("BigBro v %s\n", VERSION);

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL << CARD_SW);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    printf("Press a key to enter console\n");
    bool debug = false;
    for (int i = 0; i < 20; ++i)
    {
        if (getchar() != EOF)
        {
            debug = true;
            break;
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    if (debug)
        run_console();        // never returns
    
    printf("\nStarting application\n");
    xTaskCreate(rfid_task, "rfid_task", 10*1024, NULL, 5, NULL);
    //xTaskCreate(led_task, "led_task", 4*1024, NULL, 5, NULL);

    printf("Running display test\n");

    for (uint16_t i = 1; i < 500; i++) {
        int16_t x0 = rand() % display->width;
        int16_t y0 = rand() % display->height;
        int16_t radius = rand() % 100;
        hagl_color_t color = rand() % 0xffff;

        hagl_fill_circle(display, x0, y0, radius, color);
    }

    printf("Vibing...\n");
    while (1)
        vTaskDelay(1);
}
