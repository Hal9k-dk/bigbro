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

const auto constexpr BACKLIGHT_FADE_PERIOD = std::chrono::milliseconds(5000);
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
    state_map[State::not_allowed] = &Controller::handle_not_allowed;
    state_map[State::allowed] = &Controller::handle_allowed;

    display.clear();

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

        display.update();
        if (util::now() - last_fade_update >= BACKLIGHT_FADE_PERIOD)
        {
            last_fade_update = util::now();
            fade_to = fade_to == BACKLIGHT_FADE_MIN ? BACKLIGHT_FADE_MAX : BACKLIGHT_FADE_MIN;
            fade_backlight(fade_to, BACKLIGHT_FADE_PERIOD);
        }

        const auto old_card_id = card_id;
        card_id = get_and_clear_last_cardid();
        if (card_id != old_card_id)
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
            status_msg = format("Blocked:\n%s", user_name.c_str());
            break;

        case State::allowed:
            status_msg = format("Allowed:\n%s", user_name.c_str());
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

void Controller::handle_not_allowed()
{
    set_relay(false);
    if (switch_closed)
        check_card();
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
    //printf("Check card\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
    const auto result = Card_cache::instance().has_access(card_id);
    //printf("Got result\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
    switch (result.access)
    {
    case Card_cache::Access::Allowed:
        //printf("allowed\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
        Slack_writer::instance().send_message(format(":key: (%s) Valid card " CARD_ID_FORMAT " present, access allowed",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log(format("Valid card " CARD_ID_FORMAT " present", card_id));
        state = State::allowed;
        user_name = result.user_name;
        break;
            
    case Card_cache::Access::Forbidden:
        //printf("forbidden\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
        Slack_writer::instance().send_message(format(":bandit: (%s) Unauthorized card inserted",
                                                     get_identifier().c_str()));
        Logger::instance().log(format("Unauthorized card " CARD_ID_FORMAT " inserted", card_id));
        state = State::not_allowed;
        user_name = result.user_name;
        break;
            
    case Card_cache::Access::Unknown:
        //printf("unknown\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
        Slack_writer::instance().send_message(format(":broken_key: (%s) Unknown card " CARD_ID_FORMAT " inserted",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log_unknown_card(card_id);
        state = State::not_allowed;
        user_name.clear();
        break;
               
    case Card_cache::Access::Error:
        //printf("error\n"); vTaskDelay(2000 / portTICK_PERIOD_MS);
        Slack_writer::instance().send_message(format(":computer_rage: (%s) Internal error checking card: %s",
                                                     get_identifier().c_str(), result.error_msg.c_str()));
        state = State::not_allowed;
        user_name = "[error]";
        break;
    }
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
