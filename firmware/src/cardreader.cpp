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
    
    const int32_t c = m_serial.read();
    Serial.println(c);
    if (c > 0)
    {
        if (m_decoder.add_byte(c))
        {
            m_id = m_decoder.get_id();
            #if SERIAL_DBG
            Serial.print("ID: ");
            Serial.println(m_id);
            #endif
            if (switch_on)
            {
                #if SERIAL_DBG
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

