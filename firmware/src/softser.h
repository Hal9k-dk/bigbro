#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class Softser
{
public:
    Softser();
    void update();
    int16_t read();

private:
    SoftwareSerial m_serial;
    uint8_t pin_tx = SW_SERIAL_UNUSED_PIN;
    
    #ifdef JTAG_TEST
    uint8_t pin_rx = D3;
    #else
    uint8_t pin_rx = D5;
    #endif
};