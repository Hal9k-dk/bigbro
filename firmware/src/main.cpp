#include <Arduino.h>
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
 * Initialization code 
 */
void setup()
{
	//controller = new ACSController();
	controller = new PrinterController();
	//controller = new CameraController();
}

/**
 * Main control loop
 */
void loop()
{
	controller->update();
}