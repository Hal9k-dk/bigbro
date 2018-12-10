#pragma once

#include <controllers/base.h>
#include "cardreader.h"

class ACSController: public BaseController
{
public:
    ACSController();

    void update() override;

    bool relay_check() override;

    void test_command() override;

protected:
    CardReader reader;

    String last_card_id;
    String card_id;
    int current_user_id = 0;

    bool has_allowed_card = false;

    bool has_card();
    bool new_card();
    String get_card();
    bool card_allowed();
    bool query_permission(const String& card_id,
					  bool& allowed,
					  String& user_name,
					  int& user_id,
					  String& message);

private:
    unsigned long start_tick = millis();
    bool showing_version = true;
};