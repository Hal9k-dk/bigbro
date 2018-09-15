#pragma once

#include <Arduino.h>

class BaseController
{
public:
    uint32_t last_disp = 0;
    BaseController();
    
    void update();
};