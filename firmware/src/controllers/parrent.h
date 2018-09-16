#pragma once

#include <Arduino.h>
#include <display.h>

class Parrent
{
public:
    Parrent();
    
    uint32_t last_disp = 0;
    Display disp;
    
    virtual void update();
};