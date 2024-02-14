#include "controller.h"

#include "cJSON.h"

#include "cardreader.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "logger.h"
#include "nvs.h"
#include "reader.h"
#include "slack.h"

#ifdef DEBUG_HEAP

#include "esp_heap_trace.h"

#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM

#endif // DEBUG_HEAP

static constexpr auto UNLOCKED_ALERT_INTERVAL = std::chrono::seconds(30);

// How long to keep the door open after valid card is presented
static constexpr auto ENTER_TIME = std::chrono::seconds(6);

// How long to wait before locking when door is closed after leaving
static constexpr auto LEAVE_TIME = std::chrono::seconds(3);

static constexpr auto TEMP_STATUS_SHOWN_FOR = std::chrono::seconds(10);

// Time door is unlocked after pressing Green
static constexpr auto UNLOCK_PERIOD = std::chrono::minutes(15);
static constexpr auto UNLOCK_WARN = std::chrono::minutes(10);
// Time door is unlocked via Slack
static constexpr auto GW_UNLOCK_PERIOD = std::chrono::seconds(30);

// Time before warning when entering
static constexpr auto ENTER_UNLOCKED_WARN = std::chrono::minutes(5);

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

    display.clear();

#ifdef DEBUG_HEAP
    ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
    int loops = 0;
#endif
    
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        display.update();

        card_id = get_and_clear_last_cardid();
        if (card_id)
            Logger::instance().log(format("Card " CARD_ID_FORMAT " swiped", card_id));

        // Handle state
        auto it = state_map.find(state);
        if (it == state_map.end())
            fatal_error(format("Unhandled state %d", static_cast<int>(state)).c_str());
        const auto old_state = state;
        it->second(this);

        if (state != old_state)
            printf("STATE: %d\n", static_cast<int>(state));
        /*
        if (util::is_valid(timeout_dur))
        {
            Logger::instance().log(format("Set timeout of %d s",
                                          std::chrono::duration_cast<std::chrono::seconds>(timeout_dur).count()));
            timeout = util::now() + timeout_dur;
            timeout_dur = util::invalid_duration();
        }
        */
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
    //state = State::locked;
}

void Controller::check_card(Card_id card_id)
{
    const auto result = Card_cache::instance().has_access(card_id);
    switch (result.access)
    {
    case Card_cache::Access::Allowed:
        display.show_message("Valid card swiped");
        Slack_writer::instance().send_message(format(":key: (%s) Valid card " CARD_ID_FORMAT " swiped, unlocking",
                                                     get_identifier().c_str(), card_id));
        //state = State::timed_unlock;
        break;
            
    case Card_cache::Access::Forbidden:
        display.show_message(format("Blocked card " CARD_ID_FORMAT " swiped", card_id), COLOUR_YELLOW);
        Slack_writer::instance().send_message(format(":bandit: (%s) Unauthorized card swiped",
                                                     get_identifier().c_str()));
        Logger::instance().log(format("Unauthorized card " CARD_ID_FORMAT " swiped", card_id));
        break;
            
    case Card_cache::Access::Unknown:
        display.show_message(format("Unknown card\n" CARD_ID_FORMAT "\nswiped", card_id), COLOUR_YELLOW);
        Slack_writer::instance().send_message(format(":broken_key: (%s) Unknown card " CARD_ID_FORMAT " swiped",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log_unknown_card(card_id);
        break;
               
    case Card_cache::Access::Error:
        Slack_writer::instance().send_message(format(":computer_rage: (%s) Internal error checking card: %s",
                                                     get_identifier().c_str(), result.error_msg.c_str()));
        break;
    }
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
