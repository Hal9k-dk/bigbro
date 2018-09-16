#include <controllers/child.h>

Child::Child():
    test()
{}

void Child::update()
{
    Parrent::update();
    
    int16_t c = test.read();
    if(c >= 0)
    {
        Serial.print(c);
        cnt++;
    }

    if(cnt > 80)
    {
        cnt = 0;
        Serial.println();
    }

}