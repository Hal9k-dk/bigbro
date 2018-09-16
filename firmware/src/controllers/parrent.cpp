#include <controllers/parrent.h>

Parrent::Parrent():
    disp()
{
    Serial.begin(115200);
}

void Parrent::update()
{
    if(millis()-last_disp > 1000)
    {
        last_disp = millis();
        disp.set_status(String("Uptime"), String(millis()/1000) + " s");
    }
}