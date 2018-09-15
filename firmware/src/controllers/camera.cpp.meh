#include <controllers/camera.h>

CameraController::CameraController(const unsigned long query_interval):
  BaseController(OTA_PSW, true),
  c_query_interval(query_interval)
{}

bool CameraController::query_camera_state(bool& state, String& message) 
{
    AcsRestClient rc("camera");
    
    StaticJsonBuffer<200> jsonBuffer;
    auto& root = jsonBuffer.createObject();
    root["api_token"] = Eeprom::get_api_token();

    const auto status = rc.post(root);
    led.update();
    #if SERIAL_DBG
    Serial.print("HTTP status: ");
    Serial.println(status);
    #endif

    if (status == 200) 
    {
        String resp = rc.get_response();

        // Remove garbage (why is it there?)
        unsigned int i = 0;
        while ((resp[i] != '{') && (i < resp.length()))
            ++i;
        unsigned int j = i;
        while ((resp[j] != '}') && (j < resp.length()))
            ++j;
        resp = resp.substring(i, j+1);

        StaticJsonBuffer<200> jsonBuffer;
        auto& json_resp = jsonBuffer.parseObject(resp);
        if (!json_resp.success()) 
        {
            #if SERIAL_DBG
            Serial.println("Bad JSON:");
            Serial.println(resp);
            #endif
            message = "Bad JSON";
        } else {
            state = json_resp["state"];
        }
        
        return true;
    } else {
        String s = "Bad HTTP reply:";
        s += String(status);
        message = s;
        return false;
    }
}

bool CameraController::relay_check()
{
    const auto now = millis();
    if (now - last_check_tick > c_query_interval) {
        #if SERIAL_DBG
        Serial.println("Quering camera state");
        #endif

        last_check_tick = now;
        
        String message;
        bool status = true;
        if (!query_camera_state(status, message)) {
            // Fail-safe: activate cameras
            #if SERIAL_DBG
            Serial.println("fail-safe mode enabled");
            #endif
            display.set_status("ON, FAIL-SAFE mode", message);
            led.set_colour(CRGB::Red);
            led.set_duty_cycle(50);
            led.set_period(1);
            led.update();
            state = true;
        } else {
            state = status;
            if (status) {
                // Camera ON
                led.set_colour(CRGB::Red);
                display.set_status("ON");
            } else {
                // Camera OFF
                led.set_colour(CRGB::Green);
                display.set_status("OFF");
            }
            led.set_duty_cycle(100);
            led.update();
        }
    }
    
    return state;
}

void CameraController::test_command()
{
    Serial.println("Sending test request");
    String message;
    bool state = false;
    if (!query_camera_state(state, message))
    {
        Serial.print("Error: ");
        Serial.println(message);
    }
    else
    {
        Serial.print("State: ");
        Serial.println(state);
    }
}