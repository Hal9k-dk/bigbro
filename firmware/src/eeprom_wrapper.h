#pragma once

/** \name EEPROM Address division
 *  Offsets describe the start address of where data is stored.
 *  Size describes how many bytes are used.
 */
//@{
const uint16_t MACHINE_ID_OFFSET = 0;
const uint16_t MACHINE_ID_SIZE = 32;
const uint16_t API_TOKEN_OFFSET = MACHINE_ID_SIZE;
const uint16_t API_TOKEN_SIZE = 80;

const int MAX_SSIDS = 4;

// SSID max length according to standars is 32 bytes, 
// though a lot of access points require the last byte to be \0
const uint16_t SSID_SIZE = 32; 
const uint16_t SSID_BLOCK_OFFSET = API_TOKEN_OFFSET + API_TOKEN_SIZE; // [1B] count [32B] SSID0 .... [32B] SSIDn
/// The first byte of the block stores number of SSIDs stored. Then each 32B are SSIDs
const uint16_t SSID_BLOCK_SIZE = 1 + SSID_SIZE * MAX_SSIDS;

const uint16_t WIFI_PASS_OFFSET = SSID_BLOCK_OFFSET + SSID_BLOCK_SIZE;
const uint16_t WIFI_PASS_SIZE = 64; // Max psw size according to wpa2
const uint16_t WIFI_PASS_BLOCK_SIZE = WIFI_PASS_SIZE * MAX_SSIDS;

/// Last user to use this machine
const uint16_t LAST_USER_OFFSET = WIFI_PASS_OFFSET + WIFI_PASS_BLOCK_SIZE;
const uint8_t LAST_USER_SIZE = 18; // We'll store the truncated name 
//@}

///The total bytes of EEPROM required, used to initialize the EEPROM library
const uint16_t EEPROM_SIZE = LAST_USER_OFFSET + LAST_USER_SIZE;
const uint16_t user_input_timeout = 10000;


class Eeprom
{
public:

    /// Calls EEPROM.begin() with EEPROM_SIZE
    static void Eeprom_wrap_begin();
    
    /// @return The number of SSIDs stored
    static int get_nof_ssids();

    /// Prints the stored SSIDs to the Serial port
    static void list_ssids();

    /**
     * Stores a wifi SSID and password in EEPROM
     * @param SSID The SSID to use.
     * @param PASS The password associated with the SSID.
     */
    static void set_wifi_creds(const char* SSID, const char* PASS);

    /**
     * Sets all bytes in the SSID and pass to 0
     * @param SSID/PASS pair to clear
     */
    static void remove_wifi_creds(int index);

    /**
     * Get an SSID
     * @param index The index of the desired SSID/PASS pair
     * @return The SSID
     */
    static String get_ssid(int index);

    /**
     * Get PASS
     * @param index the index of the desired SSID/PASS pair
     * @return The password. Empty String if no password
     */
    static String get_password(int index);

    /**
     * @return The ID/name of the machine
     */
    static String get_machine_id();

    /**
     * Set the ID/name of the machine
     * @param 
     */
    static void set_machine_id(const char* id);

    static String get_api_token();
    static void set_api_token(const char* token);

    static String get_last_user();
    static void set_last_user(const char*);

private:
    static void set_ssid(const char*, int index);
    static void set_password(const char*, int index);
};
