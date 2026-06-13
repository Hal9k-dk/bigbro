#pragma once

#include "defs.h"

void init_nvs();

std::string get_acs_token();
std::string get_identifier();
std::string get_slack_token();
wifi_creds_t get_wifi_creds();
bool get_current_sense_enabled();
std::string get_mqtt_address();

void clear_wifi_credentials();
void add_wifi_credentials(const char* ssid, const char* password);
void set_acs_token(const char* token);
void set_identifier(const char* token);
void set_slack_token(const char* token);
void set_current_sense_enabled(bool enabled);
void set_mqtt_address(const char* address);

void mount_spiffs(const char* path, const char* label, size_t max_files);

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
