#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class CardReader
{
public:
    CardReader(int rx_pin, int tx_pin);
    uint8_t count = 0;
    void update();
    
private:
    SoftwareSerial m_serial;
};
