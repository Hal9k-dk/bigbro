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
    state_map[State::no_access] = &Controller::handle_no_access;
    state_map[State::allowed] = &Controller::handle_allowed;

    display.clear();

#ifdef DEBUG_HEAP
    ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_LEAKS));
    int loops = 0;
#endif
    
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        display.update();

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
    state = State::no_access;
}

void Controller::handle_no_access()
{
    set_relay(false);
    if (switch_closed)
    {
        check_card();
    }
}

void Controller::handle_allowed()
{
    set_relay(true);
    if (!switch_closed)
    {
        state = State::no_access;
    }
}

void Controller::check_card()
{
    const auto result = Card_cache::instance().has_access(card_id);
    switch (result.access)
    {
    case Card_cache::Access::Allowed:
        display.show_message("Valid card present");
        Slack_writer::instance().send_message(format(":key: (%s) Valid card " CARD_ID_FORMAT " present, access allowed",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log(format("Valid card " CARD_ID_FORMAT " present", card_id));
        state = State::allowed;
        break;
            
    case Card_cache::Access::Forbidden:
        display.show_message("Blocked card inserted", RED);
        Slack_writer::instance().send_message(format(":bandit: (%s) Unauthorized card inserted",
                                                     get_identifier().c_str()));
        Logger::instance().log(format("Unauthorized card " CARD_ID_FORMAT " inserted", card_id));
        state = State::no_access;
        break;
            
    case Card_cache::Access::Unknown:
        display.show_message(format("Unknown card\n" CARD_ID_FORMAT "\ninserted", card_id), YELLOW);
        Slack_writer::instance().send_message(format(":broken_key: (%s) Unknown card " CARD_ID_FORMAT " inserted",
                                                     get_identifier().c_str(), card_id));
        Logger::instance().log_unknown_card(card_id);
        state = State::no_access;
        break;
               
    case Card_cache::Access::Error:
        display.show_message("Internal error checking card", RED);
        Slack_writer::instance().send_message(format(":computer_rage: (%s) Internal error checking card: %s",
                                                     get_identifier().c_str(), result.error_msg.c_str()));
        state = State::no_access;
        break;
    }
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
