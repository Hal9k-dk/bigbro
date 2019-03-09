#pragma once

#include <controllers/base.h>
#include "acsrestclient.h"
#include "eeprom_wrapper.h"

class CameraController: public BaseController
{
public:
    CameraController(const unsigned long query_interval = 1000);

    bool relay_check() override;

    void test_command() override;

protected:
    bool query_camera_state(bool& state, String& message);

private:
    const unsigned long c_query_interval;
    unsigned long last_check_tick = 0;
    bool state = true;
};