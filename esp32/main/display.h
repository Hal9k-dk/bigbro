#pragma once

#include "util.h"

#include <string>
#include <vector>

#include <hagl.h>

extern uint16_t COLOUR_WHITE;
extern uint16_t COLOUR_YELLOW;

class Display
{
public:
    Display(hagl_backend_t* tft);

    hagl_backend_t* hagl()
    {
        return tft;
    }
    
    void clear();

    void update();

    /// Set the persistent status.
    void set_status(const std::string& status,
                    uint16_t colour = COLOUR_WHITE, bool large = false);

    /// Add progress message (used during boot).
    void add_progress(const std::string& status);

    /// Show a message in the status area for MESSAGE_DURATION.
    void show_message(const std::string& message, uint16_t colour = COLOUR_WHITE);

private:
    void clear_status_area();
    
    void show_text(const std::string& status, uint16_t colour, bool large);

    hagl_backend_t* tft = nullptr;
    int small_textheight = 0;
    int medium_textheight = 0;
    int large_textheight = 0;
    // Used by add_progress()
    int row = 0;
    std::vector<std::string> lines;
    // Used by set_status()
    std::string last_status;
    uint16_t last_status_colour = 0;
    bool last_status_large = false;
    // Used by show_message()
    util::time_point last_message = util::invalid_time_point();
    uint64_t uptime = 0;
    int seconds_since_status_update = 50; // first update after 10 seconds
};
