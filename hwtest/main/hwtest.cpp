#include <stdio.h>
#include <array>
#include <vector>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

constexpr const int inputs[] = {
    // Card switch
    25,
};

// Only ADC1 is supported
constexpr const int analog_inputs[] = {
    // Current sense
    3,
};

constexpr const int outputs[] = {
    // Display A0/DC
    2,
    // Display reset
    4,
    // Display backlight
    14,
    // Display CS
    15,
    // Display SCK
    18,
    // Display SDA
    23,
    // Relay
    16,
    // Ext1
    13,
    // Ext2
    26,
    // Ext3
    27,
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
    for (auto pin : inputs)
        io_conf.pin_bit_mask |= (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // Outputs
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 0;
    for (auto pin : outputs)
        io_conf.pin_bit_mask |= (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    for (auto pin : outputs)
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin), false));

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
    for (auto pin : outputs)
    {
        printf("Set pin %d HIGH\n", pin);
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin), true));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("           LOW\n");
        ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(pin), false));
    }
}

void read_inputs()
{
    for (auto pin : inputs)
    {
        printf("Pin %d is %s\n", pin,
               gpio_get_level(static_cast<gpio_num_t>(pin)) ? "HIGH" : "LOW");
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
