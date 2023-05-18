#include <stdio.h>

#include <esp_event.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#include <hagl_hal.h>
#include <hagl.h>

#include "connect.h"
#include "console.h"
#include "defs.h"
#include "hw.h"
#include "reader.h"

extern "C"
void app_main()
{
    init_hardware();

    printf("BigBro v %s\n", VERSION);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    xTaskCreate(rfid_task, "rfid_task", 10*1024, NULL, 5, NULL);

    auto display = hagl_init();

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
    //xTaskCreate(led_task, "led_task", 4*1024, NULL, 5, NULL);

    printf("Running display test\n");

    for (uint16_t i = 1; i < 500; i++) {
        int16_t x0 = rand() % display->width;
        int16_t y0 = rand() % display->height;
        int16_t radius = rand() % 100;
        hagl_color_t color = rand() % 0xffff;
            
        hagl_fill_circle(display, x0, y0, radius, color);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(connect());
    printf("Connected to WiFi\n");
    
    printf("Vibing...\n");
    while (1)
        vTaskDelay(1);
}
