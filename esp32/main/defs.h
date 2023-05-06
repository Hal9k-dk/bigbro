#pragma once

#include <driver/gpio.h>

constexpr const char* VERSION = "0.1";

constexpr const char* WIFI_KEY = "wifi";
constexpr const char* INSTANCE_KEY = "inst";
constexpr const char* ACS_TOKEN_KEY = "acs";

constexpr const auto CARD_SW = (gpio_num_t) 25;

constexpr const auto RFID_RXD = (gpio_num_t) 17;

extern char config_acs_token[];
extern char config_instance;

constexpr const char* TAG = "BIGBRO";
