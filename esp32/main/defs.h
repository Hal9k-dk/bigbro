#pragma once

#include <string>
#include <vector>

#include <driver/gpio.h>
#include <driver/uart.h>

constexpr const char* IDENTIFIER_KEY = "id";
constexpr const char* WIFI_KEY = "wifi";
constexpr const char* ACS_TOKEN_KEY = "acs";
constexpr const char* GATEWAY_TOKEN_KEY = "gwt";
constexpr const char* SLACK_TOKEN_KEY = "slt";

constexpr const auto UART_PORT_NUM = (uart_port_t) 1;
constexpr const auto PIN_CARD_SW = (gpio_num_t) 25;
constexpr const auto PIN_RELAY = (gpio_num_t) 16;

constexpr const auto PIN_RFID_RXD = (gpio_num_t) 17;

constexpr const auto PIN_BACKLIGHT = (gpio_num_t) 14;

extern char config_acs_token[];
extern char config_instance;

constexpr const char* TAG = "BIGBRO";

#define CARD_ID_FORMAT "%010llX"

using wifi_creds_t = std::vector<std::pair<std::string, std::string>>;

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
