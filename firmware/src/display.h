#pragma once

#include <U8g2lib.h>

class Display
{
public:

    ///Initialize the chat
    Display();

    ///Set a 
    void set_line(const String& text, unsigned int line_nr, bool update_display=true);
    void set_line(const char* text, unsigned int line_nr, bool update_display=true);
    void set_machine_id(const char* id);

    void set_status(const char* line1, const char* line2);
    void set_status(const String& line1, const String& line2);

    void set_status(const char* line_txt, int line_nr = 0);
    void set_status(const String& line_txt, int line_nr = 0);

    void set_network_status(const char* status);

private:    
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* m_display;
    int m_line_height = 15;
    String m_machine_id;
    String m_network_status;
    
    const static int m_line_cnt = 4;
    String m_lines[m_line_cnt];

    void update();
};
