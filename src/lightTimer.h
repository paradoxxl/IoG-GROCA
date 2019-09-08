#ifndef lightTimer_h
#define lightTimer_h

#include "Arduino.h"
#include <ezTime.h>

#include "utilityTicker.h"

struct timeEvent{
    bool turnOn;
    uint8_t hour;
    uint8_t minute;
};

class lightTimer
{
private:
    timeEvent sortedEvents[];
    Timezone *timezone;
    boolean _isOn;
    UtilityTicker ticker;
public:
    lightTimer(timeEvent events[], int length, Timezone *tz, int refreshRateMs);
    lightTimer();

    void setSchedule(timeEvent events[], int length);

    boolean isOn();

    void sortEvents(timeEvent unsortedEvents[], int length);
    void evaluateState();

    void loop();
};

#endif