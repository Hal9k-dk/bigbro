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

    ///Overload using String objects for text
    void set_line(const String& text, unsigned int line_nr, bool update_display=true);

    /**
     * Compatability function: The function uses set_line internally
     * @param id Machine ID to write on line 0
     */
    void set_machine_id(const char* id);

    /**
     * Compatability function: Sets line 2 and 3 with the strings in line1 and line2
     */
    void set_status(const char* line1, const char* line2);

    ///Overload of set_status
    void set_status(const String& line1, const String& line2);

    /**
     * Compatability overload: Sets line 2 or 3 for line_nr == 0 or 1
     * @param line_txt the text to write
     * @param line_txt the line to write to (0=2, 1=3)
     */
    void set_status(const char* line_txt, int line_nr = 0);

    ///Overload of set_status with line_nr
    void set_status(const String& line_txt, int line_nr = 0);

    /**
     * Compatability function
     * @param status text to write on line 1
     */
    void set_network_status(const char* status);

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* m_display;

    ///Height of each line
    int m_line_height = 15;
    const static int m_line_cnt = 4;

    ///Array of strings to display. One string per line.
    String m_lines[m_line_cnt];

    ///Clears and redraws the screen with the contents of m_lines
    void update();
};
