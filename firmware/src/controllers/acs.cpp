#include <controllers/acs.h>

ACSController::ACSController(): 
  reader(pin_rx, pin_tx),
  disp()
{}


void ACSController::update()
{
	//BaseController::update();
	//led.update();
	reader.update();

	if(millis() - last_disp > 1000)
	{
		last_disp = millis();
		disp.set_status(String("Millis:"), String(millis()/1000));
	}
	delay(0);
}