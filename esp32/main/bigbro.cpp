#include <stdio.h>

#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#include "cardcache.h"
#include "connect.h"
#include "console.h"
#include "controller.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "logger.h"
#include "nvs.h"
#include "reader.h"
#include "slack.h"
#include "sntp.h"

extern "C"
void app_main()
{
    init_hardware();

    printf("BigBro v %s\n", VERSION);

    mount_spiffs("/spiffs", "storage0", 10);
    list_spiffs("/spiffs/");
    
    TFT_t tft;
    Display display(&tft);
    
    display.add_progress(format("BigBro v %s", VERSION));

    display.add_progress("NVS init");
    display.add_progress("AAAAAAA");
    display.add_progress("BBBBBBB");
    display.add_progress("CCCCCCC");

    init_nvs();

    const auto wifi_creds = get_wifi_creds();
    if (!wifi_creds.empty())
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        display.add_progress("Connect to WiFi");

        int attempts_left = 5;
        bool connected = false;
        while (!connected && attempts_left)
        {
            connected = connect(wifi_creds);
            ESP_LOGI(TAG, "Connected: %d attempts: %d",
                     connected, attempts_left);
            if (!connected)
            {
                disconnect();
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                --attempts_left;
            }
        }
        if (connected)
        {
            ESP_LOGI(TAG, "Connected to WiFi");
            ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

            display.add_progress("SNTP synch");

            initialize_sntp();
            
            display.add_progress("Connected");

            Logger::instance().set_api_token(get_acs_token());
            Logger::instance().set_gateway_token(get_gateway_token());
            Slack_writer::instance().set_token(get_slack_token());
            Slack_writer::instance().set_params(false); // testing
            Card_cache::instance().set_api_token(get_acs_token());
            xTaskCreate(logger_task, "logger_task", 4*1024, NULL, 1, NULL);
            xTaskCreate(card_cache_task, "cache_task", 4*1024, NULL, 1, NULL);
            xTaskCreate(slack_task, "slack_task", 4*1024, NULL, 1, NULL);
        }
    }
    
    xTaskCreate(rfid_task, "rfid_task", 4*1024, NULL, 5, NULL);

    printf("\n\nPress a key to enter console\n");
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
        run_console(display);        // never returns

    Slack_writer::instance().send_message(format(":panopticon: BigBro frontend %s (%s)",
                                                 VERSION, get_identifier().c_str()));

    display.add_progress("Connect to WiFi");

    display.add_progress("Starting");
    Logger::instance().set_log_to_gateway(true);
    Logger::instance().log(format("ACS frontend %s (%s)", VERSION, get_identifier().c_str()));

    Controller controller(display);
    display.clear();
    controller.run(); // never returns
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
