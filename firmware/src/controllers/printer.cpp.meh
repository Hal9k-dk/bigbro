#include <controllers/printer.h>

PrinterController::PrinterController(): 
  ACSController(),
  current(pin_current, pin_debug, current_threshhold)
{
    current_sensor_present = current.sensor_present();
	if(current_sensor_present)
	{
        #if SERIAL_DBG
		Serial.println("Current sensor present");
        #endif
		// Calibrate current offset
		display.set_status("Calibrating");
		delay(2000); // Delay to allow things to settle
		current.calibrate();
		display.set_status("Ready");
	}
}

bool PrinterController::relay_check()
{
    switch(print_state)
    {
        case IDLE:
            return idle(); 
        case IN_PROGRESS:
            in_progress();
            return true;
        case COOLING:
            cooling();
            return true;
        default:
            #if SERIAL_DBG
            Serial.println("wat");
            #endif
            return false;
    }
}

bool PrinterController::idle()
{
    if(new_card())
    {
        has_allowed_card = card_allowed();
    }

    if(!has_card())
    {
        display.set_status("No card", "IDLE");
        led.set_colour(CRGB::Green);
        led.set_duty_cycle(1);
        led.set_period(10);
    }
    else
    {
        display.set_status("IDLE", 2);
    }

    if(current.is_printing() && has_allowed_card)
    {
        #if SERIAL_DBG
        Serial.println("State changed=> PRINTING");
	    #endif
        print_state = IN_PROGRESS;
        display.set_status("PRINTING");
    }

    if(has_allowed_card && !get_relay())
    {
        return true;
    }
    else if(!has_allowed_card)
    {
        return false;
    }

    // Code will reach here if the relay is on, and has_allowed_card == true
    return true;
}

void PrinterController::in_progress()
{
    if(!current.is_printing())
    {
        #if SERIAL_DBG
        Serial.println("State changed=> COOLING");
	    #endif
        end_of_print_timer = millis();
        print_state = COOLING;
        display.set_status("COOLING");
        return;
    }
    
    if(last_current_reading != current_reading)
    {
        display.set_status( String( (int16_t)(floor(current_reading + 2.5)) ) + " mA", 2);
    }
}

void PrinterController::cooling()
{
    if(millis() - end_of_print_timer > cooldown_time)
    {
        #if SERIAL_DBG
        Serial.println("State changed=> IDLE");
	    #endif
        print_state = IDLE;
    }

    uint8_t minutes_left = ceil( (double) (cooldown_time - (millis() - end_of_print_timer)) /1000.0/60.0);
    if(minutes_left != last_minutes_left)
    {
        last_minutes_left = minutes_left;
        display.set_status(String(minutes_left) + " min left", 2);
    }

    if(new_card())
    {
        has_allowed_card = card_allowed();
    }

    if(current.is_printing() && has_allowed_card)
    {
        #if SERIAL_DBG
        Serial.println("State changed=> PRINTING");
	    #endif
        print_state = IN_PROGRESS;
        display.set_status("PRINTING");
    }
}

void PrinterController::update()
{
    ACSController::update();
    current.handle();
    current_reading = current.read();
}