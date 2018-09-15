#pragma once

#include <controllers/base.h>

class CameraController: public BaseController
{
public:
    CameraController(const unsigned long query_interval = 1000);

    bool relay_check();

    void test_command();

protected:
    bool query_camera_state(bool& state, String& message);

private:
    const unsigned long c_query_interval;
    unsigned long last_check_tick = 0;
    bool state = true;
};