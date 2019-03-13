#include <controllers/base.h>
#include <controllers/acs.h>
#include <controllers/camera.h>
#include <controllers/printer.h>

/**
 * The active controller instance.
 * BaseController doesn't currently support swapping this at runtime.
 */
BaseController* controller;

/**
 * Initialization codfe 
 */
void setup()
{
#if CONTROLLER == 0
	controller = new ACSController();
#elif CONTROLLER == 1
	controller = new PrinterController();
#elif CONTROLLER == 2
	controller = new CameraController();
	
#else // Default controller
	controller = new ACSController();
	//controller = new PrinterController();
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