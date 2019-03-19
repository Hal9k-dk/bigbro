#pragma once

#include <RDM6300.h>
#include <SoftwareSerial.h>

/// A helper class for the RDM6300 RFID reader
class CardReader
{
public:

    /**
     * The RFID reader sends the read IDs over serial, and has a switch to check for the physical presence of a card.
     * @param[in] rx_pin the pin receiving data from the RDM6300
     * @param[in] tx_pin SoftwareSerial needs to be supplied a TX pin as well. It's not used.
     * @param[in] sw_pin The pin the switch is connected to. Active high.
     */
    CardReader(int rx_pin, int tx_pin, int sw_pin);

    ///Reads available serial data and determines card presence
    void update();

    /**
     * @return Card ID if card is present and correctly formatted card_id has been read.
     * @return Empty string otherwise
     */
    String get_card_id() const;
    
private:
    ///SoftwareSerial instance for reading from the sensor
    SoftwareSerial m_serial;
    
    ///Instance of the RDM6300 class, used to decode the datastream from the reader
    RDM6300 m_decoder;
    int m_switch_pin = 0;
    bool m_card_present = false;
    String m_id;
};
