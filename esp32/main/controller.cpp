#include "controller.h"

#include "cJSON.h"

#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "logger.h"
#include "nvs.h"
#include "reader.h"
#include "slack.h"

#include <thread>

#ifdef DEBUG_HEAP

#include "esp_heap_trace.h"

#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM

#endif // DEBUG_HEAP

const auto constexpr BACKLIGHT_FADE_PERIOD = std::chrono::milliseconds(2500);
const auto constexpr BACKLIGHT_FADE_MIN = 32;
const auto constexpr BACKLIGHT_FADE_MAX = 255;

Controller* Controller::the_instance = nullptr;

Controller::Controller(Display& d)
    : display(d)
{
    the_instance = this;
#ifdef DEBUG_HEAP
    ESP_ERROR_CHECK(heap_trace_init_standalone(trace_record, NUM_RECORDS));
#endif
}

Controller& Controller::instance()
{
    return *the_instance;
}

bool Controller::exists()
{
    return the_instance != nullptr;
}

void Controller::run()
{
    std::map<State, std::function<void(Controller*)>> state_map;
    state_map[State::initial] = &Controller::handle_initial;
    state_map[State::idle] = &Controller::handle_idle;
    state_map[State::not_allowed] = &Controller::handle_not_allowed_unknown;
    state_map[State::unknown] = &Controller::handle_not_allowed_unknown;
    state_map[State::allowed] = &Controller::handle_allowed;

    display.clear();

    const auto start_time = util::now();

#ifdef DEBUG_HEAP
    ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
    int loops = 0;
#endif

    auto last_fade_update = util::now();
    int fade_to = BACKLIGHT_FADE_MIN;
    fade_backlight(fade_to, BACKLIGHT_FADE_PERIOD);
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        const auto now = util::now();

        display.update();
        if (state != State::idle)
            set_backlight(255);
        else if (now - last_fade_update >= BACKLIGHT_FADE_PERIOD)
        {
            last_fade_update = now;
            fade_to = fade_to == BACKLIGHT_FADE_MIN ? BACKLIGHT_FADE_MAX : BACKLIGHT_FADE_MIN;
            fade_backlight(fade_to, BACKLIGHT_FADE_PERIOD);

            const auto since_start = now - start_time;
            if (since_start > std::chrono::minutes(15))
            {
                const auto tse = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
                const auto hms = std::chrono::hh_mm_ss<std::chrono::seconds>(tse);
                if (hms.hours() == std::chrono::hours(4) &&
                    hms.minutes() == std::chrono::minutes(44))
                {
                    Logger::instance().log("Scheduled reboot");
                    display.set_status("Rebooting", RED);
                    display.update();
                    vTaskDelay(60000 / portTICK_PERIOD_MS);
                }
            }
        }

        const auto old_card_id = card_id;
        card_id = get_and_clear_last_cardid();
        if (card_id && card_id != old_card_id)
            Logger::instance().log(format("Card " CARD_ID_FORMAT " inserted", card_id));

        switch_closed = read_switch();

        // Handle state
        auto it = state_map.find(state);
        if (it == state_map.end())
            fatal_error(format("Unhandled state %d", static_cast<int>(state)).c_str());
        const auto old_state = state;
        it->second(this);

        if (state != old_state)
            printf("STATE: %d\n", static_cast<int>(state));

        std::string status_msg;
        uint16_t status_colour = WHITE;
        switch (state)
        {
        case State::idle:
            status_msg = "Insert\ncard";
            break;

        case State::not_allowed:
            status_msg = format("Denied\n%s%s", Display::SMALL_FONT_ESC, user_name.c_str());
            status_colour = RED;
            break;

        case State::unknown:
            if (card_id)
            {
                status_msg = format("Unknown\n" CARD_ID_FORMAT, card_id);
                status_colour = RED;
            }
            break;

        case State::allowed:
            status_msg = format("Allowed\n%s%s", Display::SMALL_FONT_ESC, user_name.c_str());
            status_colour = GREEN;
            break;

        default:
            ESP_ERROR_CHECK(0);
        }
        display.set_status(status_msg, status_colour);


#ifdef DEBUG_HEAP
        ++loops;
        if (loops > 10000)
        {
            ESP_ERROR_CHECK(heap_trace_stop());
            heap_trace_dump();
        }
#endif // DEBUG_HEAP
    }
}

void Controller::handle_initial()
{
    state = State::idle;
}

void Controller::handle_idle()
{
    user_name.clear();
    set_relay(false);
    if (switch_closed)
        check_card();
}

void Controller::handle_not_allowed_unknown()
{
    set_relay(false);
    if (switch_closed)
        check_card();
    else
        state = State::idle;
}

void Controller::handle_allowed()
{
    if (!switch_closed)
    {
        state = State::idle;
        return;
    }
    set_relay(true);
}

void Controller::check_card()
{
    if (!card_id)
        return;
    const auto result = Card_cache::instance().has_access(card_id);
    switch (result.access)
    {
    case Card_cache::Access::Allowed:
        Slack_writer::instance().send_message(format(":key: (%s) Valid card " CARD_ID_FORMAT " present, access allowed",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log(format("Valid card " CARD_ID_FORMAT " present", card_id));
        state = State::allowed;
        user_name = result.user_name;
        break;
            
    case Card_cache::Access::Forbidden:
        if (state != State::not_allowed)
        {
            Slack_writer::instance().send_message(format(":bandit: (%s) Unauthorized card " CARD_ID_FORMAT " inserted",
                                                         get_identifier().c_str(),
                                                         card_id));
            Logger::instance().log(format("Unauthorized card " CARD_ID_FORMAT " inserted", card_id));
        }
        state = State::not_allowed;
        user_name = result.user_name;
        break;
            
    case Card_cache::Access::Unknown:
        if (state != State::unknown)
        {
            Slack_writer::instance().send_message(format(":broken_key: (%s) Unknown card " CARD_ID_FORMAT " inserted",
                                                         get_identifier().c_str(), card_id));
            Logger::instance().log_unknown_card(card_id);
        }
        state = State::unknown;
        user_name.clear();
        break;
               
    case Card_cache::Access::Error:
        Slack_writer::instance().send_message(format(":computer_rage: (%s) Internal error checking card",
                                                     get_identifier().c_str()));
        state = State::not_allowed;
        user_name = "[error]";
        break;
    }
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
