#ifndef UtilityTicker_h
#define UtilityTicker_h

#include "Arduino.h"

class UtilityTicker
{
private:
    unsigned long nextTick;
    bool ticked;
    int ms;

public:
    UtilityTicker(int miliseconds);
    UtilityTicker();
    bool hasTicked();

    void rst();

    void loop();
};

#endif