#include <Arduino.h>
#include "display.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

Display::Display()
{
    m_display = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    m_display->begin();
    m_display->clearBuffer();
    m_display->setDrawColor(1);
    m_display->setFont(u8g2_font_7x14B_mr);
    m_display->setFontRefHeightExtendedText();
    m_display->setFontPosTop();
    m_display->setFontDirection(0);
}

void Display::set_line(const char* text, unsigned int line_nr, bool update_display)
{
    if(line_nr > m_line_cnt-1)
    {
    #if SERIAL_DBG
        Serial.println("DISPLAY: Invalid line number");
    #endif
        return;
    }
    m_lines[line_nr] = text;

    if(update_display)
    {
        update();
    }
}

void Display::set_line(const String& text, unsigned int line_nr, bool update)
{
    set_line(text.c_str(), line_nr, update);
}

void Display::set_machine_id(const char* id)
{
    set_line(id, 0);
}

void Display::set_network_status(const char* status)
{
    set_line(status, 1);
}

void Display::set_status(const char* line1, const char* line2)
{
    set_line(line1, 2, false);
    set_line(line2, 3);
}

void Display::set_status(const String& line1, const String& line2)
{
    set_status(line1.c_str(), line2.c_str());
}

void Display::set_status(const char* line_txt, int line_nr)
{
    switch(line_nr)
    {
        case 0:
            set_line(line_txt, 2);
            break;
        case 1:
            set_line(line_txt, 3);
            break;
        default:
            break;
    }
}

void Display::set_status(const String& line_txt, int line_nr)
{
    set_status(line_txt.c_str(), line_nr);
}

void Display::update()
{
    m_display->clearBuffer();
    for(int i=0; i<m_line_cnt; i++)
    {
       m_display->drawUTF8(0, m_line_height*i, m_lines[i].c_str());
    }
    m_display->sendBuffer();
}
