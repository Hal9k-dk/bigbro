#include <Arduino.h>
#include "cardreader.h"
#include "display.h"

CardReader::CardReader(int rx_pin, int tx_pin, int sw_pin):
    m_serial(rx_pin, tx_pin),
    m_switch_pin(sw_pin)
{
    m_serial.begin(9600);
}

void CardReader::update()
{
    const bool switch_on = digitalRead(m_switch_pin);
    if (!switch_on)
    {
        m_card_present = false;
    }

    const auto c = m_serial.read();
    if (c > 0)
    {
        if (m_decoder.add_byte(c))
        {
            m_id = m_decoder.get_id();
#if SERIAL_DBG > 1
            static String last_id;
            if (m_id != last_id)
            {
                last_id = m_id;
                Serial.print("New ID: ");
                Serial.println(m_id);
            }
#endif
            if (switch_on)
            {
#if SERIAL_DBG > 1
                if (!m_card_present)
                    Serial.println("Saved");
#endif
                m_card_present = true;
            }
#if SERIAL_DBG
            else
            {
                Serial.println("Ignored - switch off");
            }
#endif
        }
    }
}

String CardReader::get_card_id() const
{
    if (m_card_present)
    {
        return m_id;
    }
    return "";
}

