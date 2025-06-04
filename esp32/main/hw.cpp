#include "hw.h"
#include "defs.h"

#include <freertos/FreeRTOS.h>
#include <driver/ledc.h>
#include <esp_log.h>

int edge_count = 0;

static void IRAM_ATTR gpio_isr_handler(void*)
{
    ++edge_count;
}

void init_hardware()
{
    // Inputs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the pins that you want to set
    io_conf.pin_bit_mask = 1ULL << PIN_CARD_SW;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf.pin_bit_mask = 1ULL << PIN_CURR_SENSE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE; // testing
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_CURR_SENSE, gpio_isr_handler, (void*) 0);
    
    // Outputs
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL << PIN_RELAY) |
       (1ULL << PIN_BACKLIGHT);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    set_relay(false);

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 1000,  // Set output frequency at 1 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = PIN_BACKLIGHT,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags = {
            .output_invert  = 1,
        }
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_ERROR_CHECK(ledc_fade_func_install(0));
}

bool read_switch()
{
    return gpio_get_level(PIN_CARD_SW);
}

void set_relay(bool on)
{
    ESP_ERROR_CHECK(gpio_set_level(PIN_RELAY, on));
}

void set_backlight(int backlight)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, backlight));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

void fade_backlight(int backlight, util::duration dur)
{
    fade_backlight(backlight, std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
}

void fade_backlight(int backlight, int ms)
{
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, backlight, ms);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
}

static bool current_sensor_active = false;

bool read_current_sensor()
{
    return current_sensor_active;
}

void cursense_task(void*)
{
    ESP_LOGI(TAG, "Current sense thread started");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Current sense edge count %d", edge_count);
    }
}
