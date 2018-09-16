
#include<softser.h>

Softser::Softser():
m_serial(pin_rx, pin_tx)
{
    m_serial.begin(9600);
}

int16_t Softser::read()
{
    if(m_serial.available())
    {
        return m_serial.read();
    }
    else
    {
        return -1;
    }
}