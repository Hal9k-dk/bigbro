#include <Arduino.h>
#include <EEPROM.h>
#include "eeprom_wrapper.h"

void Eeprom::Eeprom_wrap_begin()
{
    EEPROM.begin(EEPROM_SIZE);
    #if SERIAL_DBG
    Serial.print("EEPROM Size: "); Serial.println(EEPROM_SIZE);
    #endif
}

int Eeprom::get_nof_ssids()
{
    return EEPROM.read(SSID_BLOCK_OFFSET);
}

void Eeprom::list_ssids()
{
    uint8_t count = get_nof_ssids();

    if(count == 255 || count == 0)
    {
        Serial.println("No SSIDs configured");
        return;
    }

    for(uint8_t i=0; i < count; ++i)
    {
        Serial.print(i); Serial.print(": "); Serial.println(get_ssid(i));
        delay(0);
    }
}

String Eeprom::get_ssid(uint8_t index)
{
    String s;

    uint16_t unit_offset = 1 + (index * SSID_SIZE);

    for(uint16_t i = 0; i < SSID_SIZE; i++)
    {
        const auto c = static_cast<char>(EEPROM.read(SSID_BLOCK_OFFSET + i + unit_offset));
        if (!c)
        {
            break;
        }
        s.concat(c);
    }
    return s;
}

String Eeprom::get_password(uint8_t index)
{
    String s;

    uint16_t final_offset = WIFI_PASS_OFFSET + (WIFI_PASS_SIZE * index);

    for(uint16_t i = 0; i < WIFI_PASS_SIZE; i++)
    {
        const auto c = static_cast<char>(EEPROM.read(final_offset + i));
        if (!c)
        {
            break;
        }
        s.concat(c);
    }
    return s;
}

void Eeprom::remove_wifi_creds(uint8_t index)
{
    if(index > MAX_SSIDS)
    {
        Serial.println("Number too high. Exiting");
    }

    // Clear out requested SSID and PASS
    for(uint8_t i=0; i<SSID_SIZE; i++)
    {
        EEPROM.write( (1 + SSID_BLOCK_OFFSET + SSID_SIZE * index) + i, '\0');
    }
    for(uint8_t i=0; i<WIFI_PASS_SIZE; i++)
    {
        EEPROM.write( (WIFI_PASS_OFFSET + WIFI_PASS_SIZE * index) + i, '\0');
    }

    //Moving higher entires down
    for(uint8_t i = index+1; i<MAX_SSIDS; i++)
    {
        String SSID = get_ssid(i);
        String PASS = get_password(i);
        set_ssid(SSID.c_str(),i-1);
        set_password(PASS.c_str(), i-1);
    }

    // Clear out the now duplicate SSID and PASS
    for(uint8_t i=0; i<SSID_SIZE; i++)
    {
        EEPROM.write( (1 + SSID_BLOCK_OFFSET + SSID_SIZE * (MAX_SSIDS-1)) + i, '\0');
    }
    for(uint8_t i=0; i<WIFI_PASS_SIZE; i++)
    {
        EEPROM.write( (WIFI_PASS_OFFSET + WIFI_PASS_SIZE * (MAX_SSIDS-1)) + i, '\0');
    }
    

    EEPROM.write(SSID_BLOCK_OFFSET, EEPROM.read(SSID_BLOCK_OFFSET)-1); // Decrement stored SSID counter
    EEPROM.commit();
    Serial.println("\nSSID removed");
}

void Eeprom::set_wifi_creds(const char* SSID, const char* PASS)
{
    if (strlen(SSID) < 1)
    {
        Serial.println("ERROR: No SSID provided");
        return;
    }
    if (strlen(SSID) > SSID_SIZE)
    {
        Serial.println("ERROR: SSID too long");
        return;
    }

    if (strlen(PASS) > WIFI_PASS_SIZE)
    {
        Serial.println("ERROR: Password too long");
        return;
    }

    uint8_t count = get_nof_ssids();
    if(count == 255)
    {
        count = 0;
    }

    // Count is 1 indexed, index is 0 indexed. Hence no need to add 1
    int8_t index = count;

    if(count >= MAX_SSIDS)
    {
        Serial.println("SSID storage full, pick one to override:");
        index = 255;
        list_ssids();
        
        uint32_t timeout_start = millis();
        while(!Serial.available() && millis() - timeout_start < user_input_timeout)
        {
            delay(0);
        }

        if(Serial.available())
        {
            index = Serial.read()-'0';
        }
        else
        {
            Serial.println("User input timeout. Exiting setup");
            return;
        }

        if(index >= MAX_SSIDS)
        {
            Serial.println("Number too high. Exiting setup");
            return;
        }
        count--; // To keep SSID count the same in the end
    }

    #if SERIAL_DBG
    Serial.print("SSID: "); Serial.println(SSID);
    Serial.print("PASS: "); Serial.println(PASS);
	#endif

    Eeprom::set_ssid(SSID, index);
    Eeprom::set_password(PASS, index);

    EEPROM.write(SSID_BLOCK_OFFSET, count + 1);
    EEPROM.commit();
}

void Eeprom::set_ssid(const char* SSID, uint8_t index)
{
    uint16_t final_offset = 1 + SSID_BLOCK_OFFSET + (SSID_SIZE * index);
    uint8_t i;
    for(i=0; i<strlen(SSID); ++i)
    {
        EEPROM.write(final_offset + i, SSID[i]);
    }
    EEPROM.write(final_offset + i, '\0');
}

void Eeprom::set_password(const char* PASS, uint8_t index)
{
    uint16_t final_offset = WIFI_PASS_OFFSET + (WIFI_PASS_SIZE * index);
    uint8_t i;
    for(i=0; i<strlen(PASS); ++i)
    {
        EEPROM.write(final_offset + i, PASS[i]);
    }
    EEPROM.write(final_offset + i, '\0');
}

String Eeprom::get_machine_id()
{
    String s;
    for (uint16_t i = 0; i < MACHINE_ID_SIZE; i++)
    {
        const auto c = static_cast<char>(EEPROM.read(MACHINE_ID_OFFSET+i));
        if (!c)
        {
            break;
        }
        s.concat(c);
    }
    return s;
}

void Eeprom::set_machine_id(const char* id)
{
    if (strlen(id) > MACHINE_ID_SIZE)
    {
        Serial.println("ERROR: Machine ID too long");

        return;
    }

    uint8_t i = 0;
    while (i < strlen(id))
    {
        EEPROM.write(MACHINE_ID_OFFSET+i, id[i]);
        i++;
    }

    EEPROM.write(MACHINE_ID_OFFSET+i, 0);
    EEPROM.commit();
}


String Eeprom::get_api_token()
{
    String s;

    for(uint16_t i = 0; i < API_TOKEN_SIZE; i++)
    {
        const auto c = static_cast<char>(EEPROM.read(API_TOKEN_OFFSET+i));
        if (!c)
        {
            break;
        }
        s.concat(c);
    }

    return s;
}


void Eeprom::set_api_token(const char* token)
{
    if (strlen(token) > API_TOKEN_SIZE)
    {
        Serial.println("ERROR: API token too long");
        return;
    }

    uint8_t i = 0;
    while (i < strlen(token))
    {
        EEPROM.write(API_TOKEN_OFFSET+i, token[i]);
        i++;
    }
    
    EEPROM.write(API_TOKEN_OFFSET+i, 0);
    EEPROM.commit();
}
