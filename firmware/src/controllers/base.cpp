#include <controllers/base.h>

const char* VERSION = "0.2.2";

BaseController::BaseController(const char* psw_md5, const bool relay_upstart)
#if OTA_ENABLED
    : ota(psw_md5)
#endif
{
    Serial.begin(115200);

    auto sw_on = digitalRead(D6);
    if (sw_on)
    {
        display.set_status("ERROR!", "Remove card");
        Serial.println("A reset error occurred with a card present!");
        led.set_colour(CRGB::Red);

        while (sw_on)
        {
            Serial.print("Card switch: "); Serial.println(sw_on);
            led.set_duty_cycle(100);
            led.update();
            delay(1000);
            led.set_duty_cycle(0);
            led.update();
            delay(1000);
            sw_on = digitalRead(D6);
        }
    }
    display.set_status("", "");

#if SERIAL_DBG
    Serial.print("Machine v ");
    Serial.println(VERSION);
#endif
    
    pinMode(PIN_RELAY, OUTPUT);
    set_relay(relay_upstart);

    led.update();

    // Size of EEPROM is determined based on offsets and sizes defined in the wrapper
    Eeprom::Eeprom_wrap_begin();

    display.set_machine_id(Eeprom::get_machine_id().c_str());
    String s = "Version ";
    s += VERSION;
    display.set_status(s);

    auto rtc_info = system_get_rst_info();
    switch (rtc_info->reason)
    {
    case REASON_DEFAULT_RST:
        display.set_status("DEFAULT", 1);
        break;
    case REASON_WDT_RST:
        display.set_status("WATCHDOG", 1);
        break;
    case REASON_EXCEPTION_RST:
        display.set_status("EXCEPTION", 1);
        break;
    case REASON_SOFT_WDT_RST:
        display.set_status("SOFT WATCHDOG", 1);
        break;
    case REASON_SOFT_RESTART:
        display.set_status("SOFT RESTART", 1);
        break;
    case REASON_DEEP_SLEEP_AWAKE:
        display.set_status("DEEP AWAKE", 1);
        break;
    case REASON_EXT_SYS_RST:
        display.set_status("SYS RESET", 1);
        break;
    default:
        display.set_status(String("UNKNOWN: ")+String(rtc_info->reason), 1);
        break;
    }
    
    // Connect to WiFi network
    if (wifi_handler.init(led, display))
    {
#if OTA_ENABLED
        // Set up ota uploading
        ota.begin();
#endif
    }
    else
    {
        Serial.println("test");
    }

    delay(1000);
}

void BaseController::set_relay(bool state) 
{
    digitalWrite(PIN_RELAY, state ? HIGH : LOW);
}

bool BaseController::get_relay()
{
    return digitalRead(PIN_RELAY);
}

void BaseController::decode_line(const char* line)
{
    String ssid, pass;
    int i = 0;

    uint8_t index, ch;
    uint32_t timeout_start;

    switch (tolower(line[i]))
    {
    case 'h':
        Serial.println("Commands:");
        Serial.println("  k  Set API token");
        Serial.println("  m  Set machine ID");
        Serial.println("  d  Delete an SSID/PASS set");
        Serial.println("  w  Set wifi: w SSID PASS");
        Serial.println("  l  List SSIDs");
        Serial.println("  t  Send test request");
        Serial.println("  r  Restart the ESP");
        Serial.println("  o  Toggle relay on/off");
        break;
        
    case 'k':
        // Set API token
        while (line[++i] == ' ') {;}

        Eeprom::set_api_token(line+i);
        Serial.println("API token set");
        return;

    case 'm':
        // Set machine ID
        while (line[++i] == ' ') {;}

        Eeprom::set_machine_id(line+i);
        Serial.println("Machine ID set");
        display.set_machine_id(Eeprom::get_machine_id().c_str());
        return;

    case 'w':
        ssid = "";
        pass = "";
        // Add wifi AP
        while (line[++i] == ' ') {;} // Remove spaces before ssid

        while(line[i] != ' ' && line[i] != '\0') // As long as it's not a space
        {
            ssid += line[i++]; // Get the SSID
        }
        while(line[++i] == ' ') {;} // Remove spaces before password

        while(line[i] != ' ' && line[i] != '\0')
        {
            pass += line[i++];
        }

        Eeprom::set_wifi_creds(ssid.c_str(), pass.c_str());
        return; 

    case 'd':
        while (line[++i] == ' ')
            ;

        index = line[i++]-'0';

        Serial.print("\nConfirm deleting SSID ");Serial.print(index);Serial.println(" [Y/y]");
        Eeprom::list_ssids();
        while (line[++i] == ' ')
            ;

        timeout_start = millis();

        while(!Serial.available() && millis() - timeout_start < user_input_timeout)
        {
            delay(0);
        }

        ch = 0;
        if (Serial.available())
        {
            ch = Serial.read();
        }
        else
        {
            Serial.println("Userinput timeout");
            break;
        }

        if(ch == 'Y' || ch == 'y')
        {
            Eeprom::remove_wifi_creds(index);
        }
        else
        {
            Serial.println("Deletion cancled");
        }

        // Remove line breaks and stuff
        while(Serial.available())
        {
            Serial.read();
        }
        
        break;  

    case 't':
        test_command();
        break;
    
    case 'l':
        Eeprom::list_ssids();
        break;

    case 'r':
        ESP.reset();
        break;
        
    default:
        Serial.print("Unknown command: ");
        Serial.println(line);
        return;
    }
    // Clear buffer
    while (Serial.available())
        delay(0);
}

void BaseController::handleSerial()
{
    if (Serial.available())
    {
        yield();
        const char c = Serial.read();
        if ((c == '\r') || (c == '\n'))
        {
            if(c=='\r')
            {
                Serial.read(); // Throw away the extra newline
            }
            line[line_len] = 0;
            line_len = 0;
            decode_line(line);
        }
        else if (line_len < MAX_LINE_LENGTH)
        {
            line[line_len++] = c;
        }
        else
        {
            Serial.print("Line too long: ");
            Serial.println(line);
            line_len = 0;
        }
    }
}


void BaseController::ota_enable()
{
#if OTA_ENABLED
    m_ota_state = true;
#endif
}

void BaseController::ota_disable()
{
#if OTA_ENABLED
    m_ota_state = false;
#endif
}


void BaseController::update()
{
#if SERIAL_DBG > 5
    Serial.println("Base update");
#endif

    delay(0);

#if OTA_ENABLED
    if(m_ota_state)
    {
        this->ota.handle();
    }
#endif

    this->handleSerial();

    if (!relay_override)
        set_relay(relay_check());
}

int BaseController::log_access(const char* msg, int user_id)
{
    AcsRestClient logger("logs");
    StaticJsonBuffer<200> jsonBuffer;
    yield();
    auto& root = jsonBuffer.createObject();
    root["api_token"] = Eeprom::get_api_token();
    auto& log = root.createNestedObject("log");
    log["user_id"] = user_id;
    log["message"] = msg;
    return logger.post(root);
}
