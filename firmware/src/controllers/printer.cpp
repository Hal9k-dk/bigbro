#include <controllers/printer.h>

PrinterController::PrinterController()
    : ACSController(),
      current(PIN_CURRENT, -1, CURRENT_THRESH)
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

void PrinterController::state_change(PrintState s)
{
    print_state = s;
    switch(s)
    {
        case IDLE:
            ota_enable();
            display.set_status("IDLE", 1);
            display.set_status(Eeprom::get_last_user(), 0);
            #if SERIAL_DBG
            Serial.println("State changed=> IDLE");
	        #endif
            break;

        case INRUSH_PREVENTION: 
            m_inrush_verify_timer = millis();
            current.clear_buffer();
            current.reset_last_above();
            #if SERIAL_DBG
            Serial.println("State changed=> INRUSH_PREVENTION");
	        #endif
            break;

        case IN_PROGRESS:
            display.set_status("PRINTING", 1);
            ota_disable();

            Eeprom::set_last_user(name_trunc.c_str());
            // log_access(".Print started", current_user_id);

            #if SERIAL_DBG
            Serial.println("State changed=> IN_PROGRESS");
	        #endif
            break;
          
        case COOLING:
            end_of_print_timer = millis();
            display.set_status("COOLING", 1);

            #if SERIAL_DBG
            Serial.println("State changed=> COOLING");
	        #endif
            break;

        default:
            #if SERIAL_DBG
            Serial.println("wat");
            #endif
            break;
    }
}

bool PrinterController::idle()
{
    if(!(millis()%5000))
    {
        if(!has_card())
        {
            display.set_status(Eeprom::get_last_user(), 0);
        }
        display.set_status("IDLE", 1);
    }
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

    if(current.is_printing() && has_allowed_card)
    {
        state_change(INRUSH_PREVENTION);
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
    if(millis() - m_inrush_verify_timer < m_inrush_time)
    {
        //if(last_current_reading != current_reading)
        //{
        //    last_current_reading = current_reading;
        //    display.set_status( String( (int16_t)(floor(current_reading + 2.5)) ) + " mAI", 1);
        //}
    }
    else if(current.read() > CURRENT_THRESH)
    {
        state_change(IN_PROGRESS);
    }
    else
    {
        state_change(IDLE);
        current.reset_last_above();
    }
}

void PrinterController::in_progress()
{
    if(!current.is_printing())
    {
        state_change(COOLING);
        return;
    }
    
    //if(last_current_reading != current_reading)
    //{
    //    last_current_reading = current_reading;
    //    display.set_status( String( (int16_t)(floor(current_reading + 2.5)) ) + " mAP", 1);
    //}
}

void PrinterController::cooling()
{
    if(millis() - end_of_print_timer > cooldown_time)
    {
        state_change(IDLE);
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
        last_minutes_left = 0;
        state_change(IN_PROGRESS);
    }
}

uint32_t last_print = 0;
void PrinterController::update()
{
    #if SERIAL_DBG > 5
		Serial.println("Printer update");
    #endif
    ACSController::update();
    current.handle();
    current_reading = current.read();

    #if SERIAL_DBG > 1
    if((last_current_reading != current_reading && (millis() - last_print > 500)) || millis() - last_print > 5000)
    {
      last_print = millis();
      last_current_reading = current_reading;
      Serial.println(( String( (int16_t)(floor(current_reading + 2.5)) ) + " mA " + String(current.is_printing())));
    }
    #endif
    
}
