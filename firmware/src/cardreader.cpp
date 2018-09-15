#include "cardreader.h"
#include "display.h"

CardReader::CardReader(int rx_pin, int tx_pin):
    m_serial(rx_pin, tx_pin)
{
    m_serial.begin(9600);
    Serial.println("\nr\n");
}

void CardReader::update()
{
    if(m_serial.available())
	{
		Serial.write(m_serial.read());
		count++;
	}

	if(count > 80)
	{
		count = 0;
		Serial.println();
	}
}

