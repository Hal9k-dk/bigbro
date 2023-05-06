#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hagl_hal.h>
#include <hagl.h>

#include "console.h"
#include "defs.h"

auto display = hagl_init();

extern "C"
void app_main()
{
    printf("BigBro v %s\n", VERSION);
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
