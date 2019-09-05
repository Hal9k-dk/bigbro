#include <controllers/base.h>
#include <controllers/acs.h>
#include <controllers/camera.h>
#include <controllers/printer.h>

/*! @mainpage Documentation for the BigBro firmware
	The BigBro firmware is part of the access system used at Hal9k
	It runs on the access boxes attached to different restricted machines.

	The boxes enable or remove power from the machines, depending on access permissions.

	There are two main types of boxes the users will interact with:
	  - The basic access control box, implemented in ACSController, enables power to machines, 
	  as long as an approved card is present.
	  - The printer access control box, implemented in PrinterController, 
	  enables power to the machines if a valid card is present,
	  or if a print, started with an approved card, is currently in progress.

	  The hardware difference between the two boxes is a higher current capability on the printer box,
	  and a current sensor, as the printer box monitors current consumption of the printers, and therefore
	  passes the entire current for the machine.
	  The basic access control boxes instead use a much smaller relay, to control an external contactor, powering the machine.
*/

/**
 * The active controller instance.
 * BaseController doesn't currently support swapping this at runtime.
 */
BaseController* controller;


///Initialization code 
void setup()
{
#if CONTROLLER == 0 && defined(CONTROLLER)
#pragma message "Compiling ACSController"
	controller = new ACSController();
#elif CONTROLLER == 1
#pragma message "Compiling PrinterController"
	controller = new PrinterController();
#elif CONTROLLER == 2
#pragma message "Compiling CameraController"
	controller = new CameraController();

#else // Default controller
#pragma message "Compiling default controller"
	//controller = new ACSController();
	controller = new PrinterController();
	//controller = new CameraController();
#endif

}

/**
 * Main control loop
 */
void loop()
{
	controller->update();
}