#pragma once

const uint16_t MACHINE_ID_OFFSET = 0;
const uint16_t MACHINE_ID_SIZE = 32;
const uint16_t API_TOKEN_OFFSET = MACHINE_ID_SIZE;
const uint16_t API_TOKEN_SIZE = 80;

const uint8_t MAX_SSIDS = 4;

// SSID max length according to standars is 32 bytes, 
// though a lot of access points require the last byte to be \0
const uint16_t SSID_SIZE = 32; 
const uint16_t SSID_BLOCK_OFFSET = API_TOKEN_OFFSET + API_TOKEN_SIZE; // [1B] count [32B] SSID0 .... [32B] SSIDn
const uint16_t SSID_BLOCK_SIZE = 1 + SSID_SIZE * MAX_SSIDS;

const uint16_t WIFI_PASS_OFFSET = SSID_BLOCK_OFFSET + SSID_BLOCK_SIZE;
const uint16_t WIFI_PASS_SIZE = 64; // Max psw size according to wpa2
const uint16_t WIFI_PASS_BLOCK_SIZE = WIFI_PASS_SIZE * MAX_SSIDS;

// Last user to use this machine
const uint16_t LAST_USER_OFFSET = WIFI_PASS_OFFSET + WIFI_PASS_BLOCK_SIZE;
const uint8_t LAST_USER_SIZE = 18; // We'll store the truncated name 

const uint16_t EEPROM_SIZE = LAST_USER_OFFSET + LAST_USER_SIZE;
const uint16_t user_input_timeout = 10000;


class Eeprom
{
public:

    static void Eeprom_wrap_begin();
    
    static int get_nof_ssids();
    static void list_ssids();

    static void set_wifi_creds(const char*, const char*);
    static void remove_wifi_creds(uint8_t index);

    static String get_ssid(uint8_t index);
    static String get_password(uint8_t index);

    static String get_machine_id();
    static void set_machine_id(const char*);

    static String get_api_token();
    static void set_api_token(const char*);

    static String get_last_user();
    static void set_last_user(const char*);

private:
    static void set_ssid(const char*, uint8_t index);
    static void set_password(const char*, uint8_t index);
};
