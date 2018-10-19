#include <controllers/printer.h>

PrinterController::PrinterController(): 
  ACSController(),
  current(A0, -1, 50)
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
        display.set_status("Booted");
	}
}

bool PrinterController::relay_check()
{
    switch(print_state)
    {
        case IDLE:
            return idle(); 
        
        case INRUSH_PREVENTION:
            inrush_prevension();
            return true;

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
        if(has_card())
        {   
            has_allowed_card = card_allowed();
        }
    }

    if(!has_card())
    {
        has_allowed_card = false;
        led.set_colour(CRGB::Green);
        led.set_duty_cycle(1);
        led.set_period(10);
    }

    display.set_status("IDLE", 1);

    if(current.is_printing() && has_allowed_card)
    {
        #if SERIAL_DBG
        Serial.println("State changed=> INRUSH_PREVENTION");
	    #endif
        print_state = INRUSH_PREVENTION;
        m_inrush_verify_timer = millis();
    }

    if(has_allowed_card)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PrinterController::inrush_prevension()
{
    while(millis() - m_inrush_verify_timer < m_inrush_time)
    {
        delay(0);
    }

    if(current.is_printing())
    {
        #if SERIAL_DBG
        Serial.println("State changed=> IN_PROGRESS");
	    #endif
        print_state = IN_PROGRESS;
        display.set_status("PRINTING", 1);
        ota_disable();
    }
    else
    {
        #if SERIAL_DBG
        Serial.println("State changed=> IDLE");
	    #endif
        print_state = IDLE;
        current.reset_last_above();
    }
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
        display.set_status("COOLING", 1);
        return;
    }
    
    if(last_current_reading != current_reading)
    {
        //display.set_status( String( (int16_t)(floor(current_reading + 2.5)) ) + " mA", 1);
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
        ota_enable();
    }

    uint8_t minutes_left = ceil( (double) (cooldown_time - (millis() - end_of_print_timer)) /1000.0/60.0);
    if(minutes_left != last_minutes_left)
    {
        last_minutes_left = minutes_left;
        display.set_status("COOLING " + String(minutes_left) + " min", 1);
    }

    if(new_card())
    {
        if(has_card())
        {
            has_allowed_card = card_allowed();
        }
        else
        {
            has_allowed_card = false;
        }
    }

    if(current.is_printing() && has_allowed_card)
    {
        #if SERIAL_DBG
        Serial.println("State changed=> PRINTING");
	    #endif
        last_minutes_left = 0;
        print_state = IN_PROGRESS;
        display.set_status("PRINTING", 1);
    }
}

void PrinterController::update()
{
    ACSController::update();
    current.handle();
    current_reading = current.read();
}