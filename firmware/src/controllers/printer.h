#pragma once

#include <controllers/acs.h>
#include <currentsens.h>
class PrinterController: public ACSController
{
public:
    PrinterController();

    void update() override;
    bool relay_check() override;

private:
    Current current;

    bool current_sensor_present = false;

    uint32_t        last_calibrate, last_minutes_left;
    uint32_t        end_of_print_timer;
    const uint32_t  cooldown_time = 5*60*1000; // 5 minutes
    uint32_t        last_current_reading, current_reading;

    enum PrintState { IDLE, IN_PROGRESS, COOLING };
    PrintState print_state = IDLE;

    unsigned long start_tick = millis();
    bool showing_version = true;

    bool idle();
    void in_progress();
    void cooling();
};