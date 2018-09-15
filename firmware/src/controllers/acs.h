#pragma once

#include <controllers/base.h>
#include <cardreader.h>
#include <display.h>

class ACSController: public BaseController
{
public:
    ACSController();
    void update();

protected:
    CardReader reader;
    Display disp;

    uint8_t pin_tx = SW_SERIAL_UNUSED_PIN, pin_rx = D5, pin_switch = D6;

private:
    unsigned long start_tick = millis();
    bool showing_version = true;
};