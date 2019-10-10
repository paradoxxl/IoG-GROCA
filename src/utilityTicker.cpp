#include "Arduino.h"
#include "utilityTicker.h"

UtilityTicker::UtilityTicker(){}
UtilityTicker::UtilityTicker(int miliseconds)
{
    nextTick = millis() + miliseconds;
}
bool UtilityTicker::hasTicked()
{
    if (ticked)
    {
        ticked = !ticked;
        return true;
    }
    return ticked;
}

void UtilityTicker::rst()
{
    nextTick = millis() + ms;
}

void UtilityTicker::loop()
{
    if (millis() >= nextTick)
    {
        nextTick = millis() + ms;
        ticked = true;
    }
}