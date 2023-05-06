#pragma once

#include "RDM6300.h"

RDM6300::Card_id get_and_clear_last_cardid();

void rfid_task(void*);
