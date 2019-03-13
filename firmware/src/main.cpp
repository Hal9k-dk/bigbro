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