#pragma once

#include "util.h"

void init_hardware();

bool read_switch();

void set_relay(bool on);

void set_backlight(int backlight);

void fade_backlight(int backlight, int ms);
void fade_backlight(int backlight, util::duration dur);

bool read_current_sensor();
