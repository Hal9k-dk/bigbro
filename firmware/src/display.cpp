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

void Display::set_machine_id(const char* id)
{
    m_machine_id = id;
    update();
}

void Display::set_network_status(const char* status)
{
    m_network_status = status;
    update();
}

void Display::set_user(const String& user)
{
    m_user = user;
    update();
}

void Display::set_state(const String& status)
{
    m_state = status;
    update();
}

void Display::set_debug_info(const String& txt)
{
    m_debug_info = txt;
    update();
}

void Display::set_status(const char* line1, const char* line2)
{
    m_status1 = line1;
    m_status2 = line2;
    update();
}

void Display::set_status(const String& line1, const String& line2)
{
    set_status(line1.c_str(), line2.c_str());
}

void Display::set_status(const char* line_txt, int line_nr)
{
    if (line_nr)
        m_status2 = line_txt;
    else
        m_status1 = line_txt;
}

void Display::set_status(const String& line_txt, int line_nr)
{
    set_status(line_txt.c_str(), line_nr);
}

void Display::update()
{
    m_display->clearBuffer();
    m_display->drawUTF8(0, 0, (m_machine_id + String(" ") + m_network_status).c_str());
    m_display->drawUTF8(0, m_line_height*1, m_user.c_str());
    m_display->drawUTF8(0, m_line_height*2, m_status1.c_str());
    m_display->drawUTF8(0, m_line_height*3, m_status2.c_str());
    m_display->drawUTF8(0, m_line_height*4, (m_state + String(" ") + m_debug_info).c_str());
    m_display->sendBuffer();
}
