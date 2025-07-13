#pragma once

#include <RDM6300.h>

#include "cardcache.h"
#include "util.h"

#include <string>

class Display;

class Controller
{
public:
    Controller(Display& display);

    ~Controller() = default;
    
    static Controller& instance();

    static bool exists();

    void run();

private:
    using Card_id = RDM6300::Card_id;

    enum class State {
        initial,
        /// No card present
        idle,
        /// Last card was invalid
        not_allowed,
        /// Last card was unknown
        unknown,
        /// Valid card present
        allowed,
        /// No card, load is off (only used in current sense mode)
        powering_off,
    };

    void handle_initial();
    void handle_idle();
    void handle_not_allowed_unknown();
    void handle_allowed();
    void handle_powering_off();

    void check_card();

    static Controller* the_instance;
    Display& display;
    State state = State::initial;
    Card_id card_id = 0;
    bool switch_closed = false;
    std::string user_name;
    util::time_point last_load_on_time;
};

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
