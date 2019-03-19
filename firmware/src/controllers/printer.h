#pragma once

#define PIN_CURRENT A0

#ifndef CURRENT_THRESH
    #define CURRENT_THRESH 50 // Threshhold current consumption for "printing"
#endif

#include <controllers/acs.h>
#include <currentsens.h>

/**
 * Printer control class for controllers. Implements the functionality to read current readings and determine printer states based on it.
 */
class PrinterController: public ACSController
{
public:
    PrinterController();

    /**
     * Control loop, samples the current sensor and reads peak to peak current value.
     * Calls ACSController::update() on each run as well.
     */
    void update() override;
    
    /**
     * Returns the state of the relay
     * @return true if the card inserted has permission, and the printer is idle.
     * @return true if the printer is currently printing.
     * @return true if the printer is cooling down.
     */
    bool relay_check() override;

private:
    Current current;

    bool current_sensor_present = false;

    ///Used to keep track of last time we calibrated.
    uint32_t        last_calibrate;

    ///Last "minutes left" we displayed on the display during cooling.
    uint8_t        last_minutes_left;

    ///Used to keep track of when a print ended [milliseconds]
    uint32_t        end_of_print_timer;

    ///Used to keep track of when the printer was first considered printing. [milliseconds]
    uint32_t        m_inrush_verify_timer;

    ///Time the printer should be allowed to cool after a finished print. [milliseconds]
    const uint32_t  cooldown_time = 9*60*1000;

    ///Time to wait before verifying the printer is actually printing, and that we didn't trigger on inrush current. [milliseconds]
    const uint32_t  m_inrush_time = 5*1000;

    ///Variables used to store current reading, and the last reading we used. [milliseconds]
    uint32_t        last_current_reading, current_reading;

    /**
     * States of the printer.
     * 
     * State | Description
     * ----- | -----------
     * IDLE | The printer isn't doing anything. It'll be on if an aproved card is inserted, and off if not
     * INRUSH_PREVENTION | The current sensor sensed current consumption over the treshhold. Verify that it wasn't inrush current
     * IN_PROGRESS | The printer is considered printing. The current is regularly over the threshhold.
     * COOLING | The printer is considered done, and is allowed time to cool down before power is removed.
     */
    enum PrintState { IDLE, INRUSH_PREVENTION, IN_PROGRESS, COOLING };
    PrintState print_state = IDLE;

    /** 
     * Run if print_state == IDLE.
     * Checks for cards and if the printer is considered printing.
     * Changes state to INRUSH_PREVENTION if the printer is considered printing.
     * @return true if an allowed card is inserted
     */
    bool idle();

    /**
     * Waits m_inrush_time and checks if the current consumption is still over the threshhold.
     * If it is, it changes state to IN_PROGRESS.
     * If it isn't, it resets the current handler and changes state to IDLE
     */
    void inrush_prevension();

    /**
     * Checks that the printer is considered printing.
     * Changes state to COOLING if it isn't.
     */
    void in_progress();

    /**
     * Checks if an allowed card is present and the printer is considered printing
     * Changes state to IN_PROGRESS if that's the case
     * Changes sate to IDLE if that doesn't happen before the cooldown_time expires
     */
    void cooling();

    
    ///Handles state changes. Code to be run only on state changes is placed here.
    void state_change(PrintState s);
};