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
        ticked = false;
        return true;
    }
    return false;
}

void UtilityTicker::rst()
{
    nextTick = millis() + ms;
    ticked = false;
}

void UtilityTicker::loop()
{
    if (millis() >= nextTick)
    {
        nextTick = millis() + ms;
        ticked = true;
    }
}
