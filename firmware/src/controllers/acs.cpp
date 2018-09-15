#include <controllers/acs.h>

ACSController::ACSController():
BaseController()
{}


void ACSController::update()
{
	BaseController::update();
	//led.update();
	delay(0);
}