#pragma once

#include <driver/gpio.h>

constexpr const char* VERSION = "0.1";

constexpr const char* WIFI_KEY = "wifi";
constexpr const char* INSTANCE_KEY = "inst";
constexpr const char* ACS_TOKEN_KEY = "acs";

constexpr const auto PIN_CARD_SW = (gpio_num_t) 25;
constexpr const auto PIN_RELAY = (gpio_num_t) 16;

constexpr const auto PIN_RFID_RXD = (gpio_num_t) 17;

extern char config_acs_token[];
extern char config_instance;

constexpr const char* TAG = "BIGBRO";
