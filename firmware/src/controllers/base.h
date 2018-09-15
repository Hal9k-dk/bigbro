#pragma once

#include <Arduino.h>
#include <cardreader.h>
#include <display.h>

class BaseController
{
public:
    uint32_t last_disp = 0;
    BaseController();

    CardReader reader;
    Display disp;

    uint8_t pin_rx = D5, pin_tx = SW_SERIAL_UNUSED_PIN, pin_switch = D6;
    
    void update();
};