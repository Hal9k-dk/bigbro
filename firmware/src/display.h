#pragma once

#include <U8g2lib.h>

class Display
{
public:

    ///Initialize the chat
    Display();

    /**
     * Set a line on the display to a string of text
     * @param text The text you want to display on this line
     * @param line_nr The line you want to show this on
     * @param update_display call update() after setting the line.
     */
    void set_line(const char* text, unsigned int line_nr, bool update_display=true);

    /// \overload
    void set_line(const String& text, unsigned int line_nr, bool update_display=true);

    /**
     * Compatability function: The function uses set_line internally
     * @param id Machine ID to write on line 0
     */
    void set_machine_id(const char* id);

    // Set line 3 and 4 with the strings in line1 and line2
    void set_status(const char* line1, const char* line2);

    /// \overload
    void set_status(const String& line1, const String& line2);

    /**
     * Compatability overload: Sets line 2 or 3 for line_nr == 0 or 1
     * @param line_txt the text to write
     * @param line_nr the line to write to (0=2, 1=3)
     */
    void set_status(const char* line_txt, int line_nr = 0);

    /// \overload
    void set_status(const String& line_txt, int line_nr = 0);

    void set_network_status(const char* status);

    void set_user(const String& txt);
    
    void set_state(const String& txt);
    
    void set_debug_info(const String& txt);
    
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* m_display = nullptr;

    /// Height of each line
    static const int m_line_height = 13;

    String m_machine_id;
    String m_network_status;
    String m_state;
    String m_debug_info;
    
    String m_user;
    String m_status1;
    String m_status2;

    /// Clears and redraws the screen with the contents of m_lines
    void update();
};
