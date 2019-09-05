#pragma once

#include <FastLED.h>

class AbstractLed
{
public:
    virtual void update() = 0;

    virtual AbstractLed& set_period(int ms) = 0;

    virtual AbstractLed& set_duty_cycle(int dc) = 0;

    virtual AbstractLed& set_colour(const CRGB& colour) = 0;
};

template<int PIN>
class Led : public AbstractLed
{
public:
    Led()
    {
        FastLED.addLeds<WS2811, PIN, GRB>(&m_led, 1).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(25);
        FastLED.show();
    }

    void update()
    {
        const auto now = millis();
        const auto elapsed = now - m_last_tick;
        if (elapsed < m_period)
        {
            return;
        }
        else
        {
            m_changed = true;
        }

        if(m_changed)
        {
            m_changed = false;
            m_last_tick = now;
            if (m_cycle >= 100)
            {
                m_cycle = 0;
            }
            if (m_cycle <= m_duty_cycle)
            {
                m_led = m_c1;
            }
            else
            {
                m_led = m_c2;
            }
            ++m_cycle;
            FastLED.show();
        }
    }

    Led& set_period(int ms)
    {
        if(m_period != ms)
        {
            m_period = ms;
            m_changed = true;
        }
        return *this;
    }

    // 1-100
    Led& set_duty_cycle(int dc)
    {
        if(m_duty_cycle != dc)
        {
            m_duty_cycle = dc;
            m_changed = true;
        }
        return *this;
    }

    Led& set_colour(const CRGB& colour)
    {
        if(m_c1 != colour)
        {
            m_c1 = colour;
            m_c2 = CRGB::Black;
            m_changed = true;
        }
        return *this;
    }

    Led& set_colours(const CRGB& colour1, const CRGB& colour2)
    {
        if(m_c1 != colour1 || m_c2 != colour2)
        {
            m_c1 = colour1;
            m_c2 = colour2;
            m_changed = true;
        }
        return *this;
    }

private:
    CRGB m_led;
    CRGB m_c1;
    CRGB m_c2;
    int m_pin = 0;
    int m_period = 1;
    int m_duty_cycle = 1;
    unsigned long m_last_tick = 0;
    int m_cycle = 0;
    bool m_changed = true;
};
