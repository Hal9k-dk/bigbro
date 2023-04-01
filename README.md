# BigBro

BigBro is the system used by Hal9K to control access to its different advanced
machines, with RFID based access control.

The system is designed to cut power to the machines if no valid key-card is present
or in the special case of a 3D printer, if no special key-card is present, 
and a print isn't ongoing.

The first version of BigBro was designed around the ESP8266, specifically the WEMOS D1 mini.
Version 2 uses ESP32 with the [EspressIf ESP-IDF](https://idf.espressif.com/).
