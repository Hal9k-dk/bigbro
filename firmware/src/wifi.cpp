#include <ESP8266WiFi.h>
#include "eeprom_wrapper.h"
#include "wifi.h"

WiFiHandler::WiFiHandler(uint8_t max_tries)
{
    m_max_tries = max_tries;
}

bool WiFiHandler::init(AbstractLed& led, Display& disp)
{    
    led.set_colour(CRGB::Orange);
    led.set_duty_cycle(10);
    led.set_period(1);

    disp.set_network_status("---");
    
    String dns_name = "machine-";
    dns_name += Eeprom::get_machine_id();
    WiFi.hostname(dns_name);
    WiFi.mode(WIFI_STA);

    uint8_t ssid_count = Eeprom::get_nof_ssids();
    bool connected = false;

#if SERIAL_DBG
	Serial.print("SSID cnt: ");
	Serial.println(ssid_count);
#endif

    if (ssid_count == 0 || ssid_count == 255)
    {
#if SERIAL_DBG
        Serial.println("No SSIDs set up");
#endif
        return false;  
    }

    for (int k = 0; k < m_max_tries && !connected; k++)
    {
#if SERIAL_DBG
        Serial.print("\nTry: "); Serial.print(k+1); Serial.print("/"); Serial.println(m_max_tries);
#endif

        for (int i = 0; i < ssid_count && !connected; i++)
        {
#if SERIAL_DBG
            Serial.print(" SSID: "); Serial.print(i+1); Serial.print("/"); Serial.println(ssid_count);
#endif
            led.update();

            String ssid = Eeprom::get_ssid(i);
            String s = "Trying ";
            s += ssid;
            disp.set_status(s.c_str());
#if SERIAL_DBG
            Serial.print(" ");Serial.println(s);
#endif

            WiFi.begin(ssid.c_str(), Eeprom::get_password(i).c_str());

            led.update();
            
            uint32_t connection_start = millis();
            while (millis() - connection_start < 10000)
            {
                delay(0);
                led.update();

                if (WiFi.status() == WL_CONNECTED)
                {
                    connected = true;
                    disp.set_status("");
                    break;
                }
            }

            if (connected)
            {
                const auto token = Eeprom::get_api_token();
                disp.set_network_status(token.length() ? "Rdy" : "Onl");

#if SERIAL_DBG
                Serial.println("");
                Serial.print("Connected to ");
                Serial.println(ssid);
#endif

                led.set_colour(CRGB::Green);
                led.set_duty_cycle(1);
                led.set_period(10);

                break;
            }
        }
    }

    led.update();
    
    // Print the IP address
    Serial.println(WiFi.localIP());
    
    led.update();

    // Set up mDNS responder
    if (!m_mdns.begin(dns_name.c_str()))
    {
        Serial.println("Error setting up mDNS responder!");
    }
    else
    {
        Serial.println("mDNS responder started");
        Serial.print("My name is ");
        Serial.println(dns_name);
    }

    return connected;
}
