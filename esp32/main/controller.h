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
        /// Power off
        no_access,
        /// Valid card present
        allowed,
    };

    void handle_initial();
    void handle_no_access();
    void handle_allowed();

    void check_card();

    static Controller* the_instance;
    Display& display;
    State state = State::initial;
    Card_id card_id = 0;
    bool switch_closed = false;
    //std::string who;
};

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
