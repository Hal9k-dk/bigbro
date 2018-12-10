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
    uint32_t        end_of_print_timer, m_inrush_verify_timer;

    const uint32_t  cooldown_time = 9*60*1000; // 9 minutes
    const uint32_t  m_inrush_time = 5*1000;    // 5 seconds

    uint32_t        last_current_reading, current_reading;

    enum PrintState { IDLE, INRUSH_PREVENTION, IN_PROGRESS, COOLING };
    PrintState print_state = IDLE;

    unsigned long start_tick = millis();
    bool showing_version = true;

    bool idle();
    void inrush_prevension();
    void in_progress();
    void cooling();

    void state_change(PrintState s);
};