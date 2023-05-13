#include <stdio.h>
#include <array>
#include <vector>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

struct pin_def
{
    int pin = 0;
    const char* name = nullptr;
};
    
constexpr const pin_def inputs[] = {
    { 17, "RFID TX" },
    { 25, "Card switch" },
};

// Only ADC1 is supported
constexpr const int analog_inputs[] = {
    // Current sense
    3,
};

constexpr const pin_def outputs[] = {
    { 2, "Display A0/DC" },
    { 4, "Display reset" },
    { 14, "Display backlight" },
    { 15, "Display CS" },
    { 18, "Display SCK" },
    { 23, "Display SDA" },
    { 16, "Relay" },
    { 13, "Ext1" },
    { 26, "Ext2" },
    { 27, "Ext3" },
};

// -- end of config

adc_oneshot_unit_handle_t adc1_handle;

void init_hw()
{
    // Inputs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 0;
    for (auto pin_def : inputs)
        io_conf.pin_bit_mask |= (1ULL << pin_def.pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // Outputs
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 0;
    for (auto pin_def : outputs)
        io_conf.pin_bit_mask |= (1ULL << pin_def.pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    for (auto pin_def : outputs)
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin_def.pin), false));

    // Analog in
    
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .clk_src = static_cast<adc_oneshot_clk_src_t>(0),
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    for (auto channel : analog_inputs)
    {
        adc_oneshot_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle,
                                                   static_cast<adc_channel_t>(channel),
                                                   &config));
    }
}

void toggle_outputs()
{
    for (auto pin_def : outputs)
    {
        printf("Set pin %2d (%s) HIGH\n", pin_def.pin, pin_def.name);
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin_def.pin), true));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf(" - LOW\n");
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin_def.pin), false));
    }
}

void read_inputs()
{
    for (auto pin_def : inputs)
    {
        printf("Pin %2d (%s) is %s\n", pin_def.pin, pin_def.name,
               gpio_get_level(static_cast<gpio_num_t>(pin_def.pin)) ? "HIGH" : "LOW");
    }
}

void read_analog_inputs()
{
    for (auto channel : analog_inputs)
    {
        int adc_raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle,
                                         static_cast<adc_channel_t>(channel),
                                         &adc_raw));
        printf("ADC1 channel %d raw data: %d\n", channel, adc_raw);
    }
}

extern "C"
void app_main(void)
{
    init_hw();

    while (1)
    {
        toggle_outputs();

        for (int i = 0; i < 10; ++i)
        {
            read_inputs();
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }

        for (int i = 0; i < 10; ++i)
        {
            read_analog_inputs();
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
    }
}
