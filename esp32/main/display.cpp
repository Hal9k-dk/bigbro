#include "connect.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "logger.h"

#include <esp_heap_caps.h>

static FontxFile small_font[2];
static FontxFile medium_font[2];

static constexpr const auto MESSAGE_DURATION = std::chrono::seconds(10);

// Top part of screen

static constexpr const int STATUS_HEIGHT = 20;

// Bottom part of screen
static constexpr const int TIME_HEIGHT = 20;

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
    lcdDrawFillRect(tft, CONFIG_WIDTH, 0,
                    CONFIG_HEIGHT - STATUS_HEIGHT - TIME_HEIGHT, STATUS_HEIGHT,
                    RED);
}

static std::vector<std::string> split(const std::string& s)
{
    std::vector<std::string> v;
    std::string::size_type curpos = 0;
    while (curpos < s.size())
    {
        const auto end = s.find('\n', curpos);
        if (end == std::string::npos)
        {
            // Last line
            v.push_back(s.substr(curpos));
            return v;
        }
        const auto line = s.substr(curpos, end - curpos);
        v.push_back(line);
        curpos = end + 1;
    }
    return v;
}

void Display::show_text(const std::string& status, uint16_t colour)
{
    const auto lines = split(status);
    auto y = CONFIG_HEIGHT -
        (STATUS_HEIGHT
         + (CONFIG_HEIGHT - STATUS_HEIGHT - TIME_HEIGHT)/2
         - lines.size()/2*medium_textheight
         - medium_textheight/2);
    for (const auto& line : lines)
    {
        const auto w = text_width(medium_font, line);
        if (w > CONFIG_WIDTH)
            printf("String '%s' is too wide\n", line.c_str());
        const auto x = CONFIG_WIDTH/2 - w/2;
        lcdDrawString(tft, medium_font, y, x,
                      reinterpret_cast<const uint8_t*>(line.c_str()), colour);
        y -= medium_textheight;
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
    /*
    time_t current = 0;
    time(&current);
    if (current != last_clock)
    {
        // Update time
        char stamp[Logger::TIMESTAMP_SIZE];
        last_clock = Logger::make_timestamp(stamp);
        tft.fillRect(0, CONFIG_HEIGHT - TIME_HEIGHT, CONFIG_WIDTH, TIME_HEIGHT, BLACK);
        tft.setTextColor(Gateway::instance().get_allow_open() ? CYAN : YELLOW);
        tft.setFreeFont(time_font);
        if (clock_x == 0)
        {
            const auto w = tft.textWidth(stamp, GFXFF);
            clock_x = CONFIG_WIDTH/2 - w/2;
        }
        tft.drawString(stamp, clock_x, CONFIG_HEIGHT - TIME_HEIGHT, GFXFF);
        ++uptime;
        if (!(uptime % 64))
        {
            // Dump mem use
            ESP_LOGI(TAG, "Uptime %" PRIu64 " memory %zu",
                     uptime,
                     heap_caps_get_free_size(MALLOC_CAP_8BIT));
        }
        ++seconds_since_status_update;
        if (seconds_since_status_update >= 60)
        {
            // Update status bar
            seconds_since_status_update = 0;
            const uint64_t days = uptime/(24*60*60);
            int minutes = (uptime - days*24*60*60)/60;
            const int hours = minutes/60;
            minutes -= hours*60;
            const auto ip = get_ip_address();
            char ip_buf[4*(3+1)+1];
            esp_ip4addr_ntoa(&ip, ip_buf, sizeof(ip_buf));
            const int mem = heap_caps_get_free_size(MALLOC_CAP_8BIT)/1024;
            const auto status = format("V%s - %s - %" PRIu64 "d%0d:%02d - M%d",
                                       VERSION, ip_buf,
                                       days, hours, minutes,
                                       mem);
            tft.fillRect(0, 0, CONFIG_WIDTH, STATUS_HEIGHT, BLACK);
            tft.setTextColor(OLIVE);
            tft.setFreeFont(status_font);
            tft.drawString(status.c_str(), 0, 0, GFXFF);
        }
    }
    */
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
