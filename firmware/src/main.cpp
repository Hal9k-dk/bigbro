
//#define SHIT

#ifndef SHIT

#include "main.h"

ACSController controller;
//PrinterController controller;
//CameraController controller;

//BaseController controller;

void setup()
{
}
void loop()
{
	controller.update();
}



#else
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial m_serial(D5, SW_SERIAL_UNUSED_PIN);

void setup()
{
	Serial.begin(115200);
	m_serial.begin(9600);
	Serial.println("\nSoftware serial test started");
}

uint8_t count = 0;
void loop() 
{
	
    Serial.print(m_serial.read());
    count++;

  if(count > 40)
  {
    count = 0;
    Serial.println();
  }
}
#endif