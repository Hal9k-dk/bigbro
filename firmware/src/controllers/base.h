#pragma once

#include <display.h>
#include <led.h>
#include <wifi.h>
#include <ota.h>

#define PIN_LED         D7
#define PIN_RELAY       D8
#define MAX_LINE_LENGTH 100

#define OTA_PSW "5f6cbcdc4e0b198694fb67b0ac3f3c1d"

#ifndef OTA_PSW
  #define OTA_PSW "202cb962ac59075b964b07152d234b70" // 123 default
#endif

/**
 * Base Class for controllers. Implements the common functionality all controllers use.
 */
class BaseController
{
public:
    /**
     * @param psw_md5 MD5 of OTA password. Set to empty string to disable authentication.
     * @param relay_upstart The start of the relay at boot.
     */
    BaseController(const char* psw_md5 = OTA_PSW, const bool relay_upstart = false);

    /**
     * Control loop, handles serial commands, led updates and more.
     * Classes that extends this class and overrides this function should call this or reimplmentt it. 
     */
    virtual void update();

    /**
     * Called on each update to determine the state of the relay.
     */
    virtual bool relay_check() = 0; // Define as pure virtual function

    /**
     * Implements the functionality of the t command.
     */
    virtual void test_command() = 0;

protected:
    /// Display used for showing status information
    Display display;
    /// Multi-colr case led 
    Led<PIN_LED> led;
    WiFiHandler wifi_handler;
    /**
     * Over-The-Air update handler.
     * 
     * The default password is 123.
     * Beaware that the OTA mechanism works by creating a server on you computer and then instructing the ESP8266 to download the new firmware from that server.\n
     * Note: password hashes can be computed using the following commands: echo -n 123 | md5sum
     */
    OTA ota;

    /**
     * @param msg The text that will be saved on the backend (panopticon)
     * @param user_id The ID of the user that caused the messaged to be logged
     */
    int log_access(const char* msg, int user_id);

    /**
     * Used for toggling the state of the relay
     * @param state relay is closed when true
     */
    void set_relay(bool state);

    /**
     * Returns the state of the relay
     * @return true if the relay is closed 
     */
    bool get_relay();

    /**
     * Enables OTA updates for the board
     */
    void ota_enable();

    /**
     * Disables OTA updates for the board
     */
    void ota_disable();

private:
    /**
     * Read and executes a command from line.
     */
    void decode_line(const char* line);

    /**
     * Reads a line from USB serial into line buffer, and call decode_line.
     * Lines can be terminted by either \\r\\n or \\n.
     */
    void handleSerial();

    /**
     * Line buffer for handleSerial and decode_line 
     */
    char line[MAX_LINE_LENGTH + 1];

    /**
     * The amount of characters written into line last time handleSerial has called 
     */
    int line_len = 0;

    /**
     * Relay state. false = open, true = closed
     */
    bool m_relay_state = 0;

    /**
     * OTA enabled state
     */
    bool m_ota_state = 1;
};