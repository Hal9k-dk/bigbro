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
    };

    void handle_initial();

    void check_card(Card_id card_id);

    static Controller* the_instance;
    Display& display;
    State state = State::initial;
    Card_id card_id;
    std::string who;
};

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
