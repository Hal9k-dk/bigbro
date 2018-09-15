#include <controllers/base.h>

const char* VERSION = "0.1.2";

BaseController::BaseController()
{
	Serial.begin(115200);
}

void BaseController::update()
{
	delay(0);
}