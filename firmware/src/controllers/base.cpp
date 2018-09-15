#include <controllers/base.h>

const char* VERSION = "0.1.2";

BaseController::BaseController(): 
  reader(pin_rx, pin_tx),
  disp()
{
	Serial.begin(115200);
}

void BaseController::update()
{
	reader.update();

	if(millis() - last_disp > 1000)
	{
		last_disp = millis();
		disp.set_status(String("Millis:"), String(millis()/1000));
	}
	delay(0);
}