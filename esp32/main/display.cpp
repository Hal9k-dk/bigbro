#include "connect.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "logger.h"
#include "nvs.h"

#include <esp_heap_caps.h>

static FontxFile small_font[2];
static FontxFile medium_font[2];

static constexpr const auto MESSAGE_DURATION = std::chrono::seconds(10);

// Bottom part of screen
static constexpr const int TIME_HEIGHT = 16;

// Top part of screen
static constexpr const int STATUS_HEIGHT = CONFIG_WIDTH - TIME_HEIGHT;

const char* Display::SMALL_FONT_ESC = "\001";

Display::Display(TFT_t* tft)
    : tft(tft)
{
    spi_master_init(tft, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO,
                    CONFIG_TFT_CS_GPIO, CONFIG_DC_GPIO, 
                    CONFIG_RESET_GPIO, CONFIG_BL_GPIO, -1, -1, -1, -1, -1);
    lcdInit(tft, 0x7735, CONFIG_WIDTH, CONFIG_HEIGHT,
            CONFIG_OFFSETX, CONFIG_OFFSETY);
    InitFontx(small_font, "/spiffs/ILGH16XB.FNT", ""); // 8x16Dot Gothic
    InitFontx(medium_font, "/spiffs/ILGH24XB.FNT", ""); // 12x24Dot Gothic

    uint8_t buffer[FontxGlyphBufSize];
    GetFontx(small_font, 0, buffer, nullptr, &small_textheight);
    GetFontx(medium_font, 0, buffer, nullptr, &medium_textheight);
    lcdSetFontDirection(tft, DIRECTION90);
    clear();
}

void Display::clear()
{
    lcdFillScreen(tft, BLACK);
    lines.clear();
    row = 0;
}

static int text_width(FontxFile* fx, const std::string& s)
{
    int w = 0;
    for (char c : s)
    {
        uint8_t buffer[FontxGlyphBufSize];
        uint8_t char_width;
        GetFontx(fx, (uint8_t) c, buffer, &char_width, nullptr);
        w += char_width;
    }
    return w;
}

void Display::add_progress(const std::string& status)
{
    const auto w = text_width(small_font, status);
    if (w > CONFIG_WIDTH)
        printf("String '%s' is too wide\n", status.c_str());
    const auto x = CONFIG_WIDTH/2 - w/2;
    lcdDrawString(tft, small_font, CONFIG_HEIGHT - (row + 1) * small_textheight, x,
                  reinterpret_cast<const uint8_t*>(status.c_str()), WHITE);
    lines.push_back(status);
    ++row;
    if (row * small_textheight <= CONFIG_HEIGHT)
        return; // still room for more
    // Out of room, scroll up
    lines.erase(lines.begin());
    --row;
    clear();
    for (int i = 0; i < lines.size(); ++i)
    {
        const auto w = text_width(small_font, status);
        const auto x = CONFIG_WIDTH/2 - w/2;
        lcdDrawString(tft, small_font, CONFIG_HEIGHT - (i + 1) * small_textheight, x,
                      reinterpret_cast<const uint8_t*>(lines[i].c_str()), WHITE);
    }
}

void Display::set_status(const std::string& status, uint16_t colour)
{
    if (status != last_status)
    {
        printf("set_status: new %s\n", status.c_str());
        clear_status_area();
        last_status = status;
        last_status_colour = colour;
        show_text(status, colour);
    }
}

void Display::clear_status_area()
{
    lcdDrawFillRect(tft, TIME_HEIGHT, 0,
                    CONFIG_WIDTH, CONFIG_HEIGHT,
                    BLACK);
}

void Display::check_escapes(std::string& line, int& height)
{
    const auto esc_pos = line.find(SMALL_FONT_ESC);
    if (esc_pos != 0)
        return;
    line = line.substr(1);
    height = small_textheight;
}

// Returns list of (line, pixel height)
std::vector<std::pair<std::string, int>> Display::split(const std::string& s)
{
    std::vector<std::pair<std::string, int>> v;
    int height = medium_textheight;
    std::string::size_type curpos = 0;
    while (curpos < s.size())
    {
        const auto end = s.find('\n', curpos);
        if (end == std::string::npos)
        {
            // Last line
            std::string line = s.substr(curpos);
            check_escapes(line, height);
            v.push_back(std::make_pair(line, height));
            return v;
        }
        auto line = s.substr(curpos, end - curpos);
        check_escapes(line, height);
        v.push_back(std::make_pair(line, height));
        curpos = end + 1;
    }
    return v;
}

void Display::show_text(const std::string& status, uint16_t colour)
{
    const auto lines = split(status);
    auto y = CONFIG_HEIGHT/2 + (lines.size() - 1)*medium_textheight/2;
    for (const auto& e : lines)
    {
        auto line = e.first;
        const auto font = e.second == medium_textheight ? medium_font : small_font;
        int w = text_width(font, line);
        if (w > CONFIG_WIDTH)
        {
            printf("String '%s' is too wide\n", line.c_str());
            auto orig_line = line;
            while (w > CONFIG_WIDTH)
            {
                orig_line = orig_line.substr(0, orig_line.size() - 1);
                line = orig_line + "..";
                w = text_width(font, line);
            }
            printf("Using %s\n", line.c_str());
        }
        const auto x = CONFIG_WIDTH/2 - w/2;
        lcdDrawString(tft, font, y, x,
                      reinterpret_cast<const uint8_t*>(line.c_str()), colour);
        y -= e.second;
    }
}

void Display::show_message(const std::string& message, uint16_t colour)
{
    last_message = util::now();
    clear_status_area();
    show_text(message, colour);
}

void Display::update()
{
    if (util::is_valid(last_message) &&
        util::now() - last_message >= MESSAGE_DURATION)
    {
        // Clear message, show last status
        last_message = util::invalid_time_point();
        clear_status_area();
        show_text(last_status, last_status_colour);
    }
    time_t current = 0;
    time(&current);
    if (current == last_clock)
        return;

    // Update time
    char stamp[Logger::TIMESTAMP_SIZE];
    last_clock = Logger::make_timestamp(stamp, false);
    lcdDrawFillRect(tft, 0, clock_x,
                    TIME_HEIGHT, CONFIG_HEIGHT - clock_x, BLACK);
    if (clock_x == 0)
    {
        const auto w = text_width(small_font, stamp);
        clock_x = CONFIG_WIDTH/2 - w/2;
        cur_sense_x = text_width(small_font, "W") + 3;
    }
    lcdDrawString(tft, small_font, 0, clock_x,
                  reinterpret_cast<const uint8_t*>(stamp), CYAN);
    // WiFi indicator
    if (is_wifi_connected())
        lcdDrawString(tft, small_font, 0, 0,
                      reinterpret_cast<const uint8_t*>("W"), CYAN);
    else
        lcdDrawString(tft, small_font, 0, 0,
                      reinterpret_cast<const uint8_t*>("-"), RED);
    // Current sense indicator
    if (get_current_sense_enabled())
    {
        if (read_current_sensor())
            lcdDrawString(tft, small_font, 0, cur_sense_x,
                          reinterpret_cast<const uint8_t*>("C"), BLUE);
        else
            lcdDrawString(tft, small_font, 0, cur_sense_x,
                          reinterpret_cast<const uint8_t*>("-"), WHITE);
    }
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
