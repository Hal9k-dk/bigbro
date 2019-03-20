#include <Arduino.h>
#include "currentsens.h"

Current::Current(uint8_t pin, int8_t debug_pin, uint16_t threshold)
{
    m_pin = pin;
    m_threshold = threshold;

    pinMode(m_pin, INPUT);
    
    if(debug_pin >= 0)
    {
        m_debug_pin = debug_pin;
        pinMode(m_debug_pin, OUTPUT);
    }
}

bool Current::sensor_present()
{
    uint16_t average_reading = 0;
    for(uint8_t i = 0; i < 50; i++)
    {
        average_reading += analogRead(m_pin);
        delay(5); // Delay serves as a yield as well
    }
    average_reading /= 50;
    if(average_reading > 500 && average_reading < 600)
    {
        return true;
    }
    return false;
}

bool Current::is_printing()
{
    int16_t current = read();

    #if SERIAL_DBG > 2
    Serial.print("reading: ");Serial.println(current);
    #endif

    if(current > m_threshold)
    {
        m_last_above_thresh = millis();
    }
    else if(current < m_threshold && (millis() - m_last_above_thresh) > m_max_below_time)
    {
        return false;
    }
    
    return true;
}

void Current::reset_last_above()
{
    m_last_above_thresh = millis() - (m_max_below_time +1);
    #if SERIAL_DBG
    Serial.println("Reset last above");
    #endif
}

void Current::clear_buffer()
{
    for(int i=0; i<m_raw_sample_size; i++)
    {
        m_raw_samples[i] = 0;
    }
    #if SERIAL_DBG
    Serial.println("Clear Buffer");
    #endif
}

void Current::handle()
{
    if(m_debug_pin >= 0)
    {
        // Send a pulse to show how often analog is sampled.
        digitalWrite(m_debug_pin, HIGH);
        digitalWrite(m_debug_pin, LOW);
    }
    sample();
}

void Current::calibrate()
{
    uint32_t calibrate_start = millis();
    while(millis() - calibrate_start < 5000)
    {
        delay(0);
        handle();
    }
    m_error = m_p2p();
}

int Current::read()
{
    return (m_p2p() - m_error) * raw_to_mA_factor;
}

void Current::sample()
{
    if(m_raw_sample_offset >= m_raw_sample_size)
    {
        m_raw_sample_offset = 0;
    }

    if(millis() - m_last_sample > m_sample_period)
    {   
        uint16_t reading = analogRead(A0);
        //Serial.print(" " + String(reading)+ " ");

        m_last_sample = millis();
        m_raw_samples[m_raw_sample_offset++] = reading;
    }
}

// Private functions

uint16_t Current::m_p2p()
{
    uint16_t max = 0, min = -1;

    for(uint16_t i = 0; i < m_raw_sample_size; i++)
    {
        if(m_raw_samples[i] < min)
        {
            min = m_raw_samples[i];
        }
        else if(m_raw_samples[i] > max)
        {
            max = m_raw_samples[i];
        }
        delay(0);
    }
    return max - min;
}