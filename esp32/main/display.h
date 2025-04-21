#pragma once

#include "util.h"

#include <string>
#include <vector>

#include <ili9340.h>

class Display
{
public:
    static const char* SMALL_FONT_ESC;

    Display(TFT_t* tft);

    TFT_t& get_tft()
    {
        return *tft;
    }
    
    void clear();

    void update();

    /// Set the persistent status.
    void set_status(const std::string& status,
                    uint16_t colour = WHITE);

    /// Add progress message (used during boot).
    void add_progress(const std::string& status);

    /// Show a message in the status area for MESSAGE_DURATION.
    void show_message(const std::string& message, uint16_t colour = WHITE);

private:
    void clear_status_area();
    
    void show_text(const std::string& status, uint16_t colour);

    void check_escapes(std::string& line, int& height);

    std::vector<std::pair<std::string, int>> split(const std::string& s);

    TFT_t* tft = nullptr;
    uint8_t small_textheight = 0;
    uint8_t medium_textheight = 0;
    // Used by add_progress()
    int row = 0;
    std::vector<std::string> lines;
    // Used by set_status()
    std::string last_status;
    uint16_t last_status_colour = 0;
    // Used by show_message()
    util::time_point last_message = util::invalid_time_point();
    time_t last_clock = 0;
    int clock_x = 0;
    int cur_sense_x = 0;
};
