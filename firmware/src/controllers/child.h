#pragma once

#include <controllers/parrent.h>
#include <softser.h>

class Child: public Parrent
{
public:
    Child();
    virtual void update();

    uint8_t cnt = 0;

    Softser test;
};