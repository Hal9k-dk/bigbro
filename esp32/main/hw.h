#pragma once

void init_hardware();

void set_relay(bool on);

void set_backlight(int backlight);

void fade_backlight(int backlight, int ms);
