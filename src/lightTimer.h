#ifndef lightTimer_h
#define lightTimer_h

#include "Arduino.h"
#include <ezTime.h>

#include "utilityTicker.h"
#include "timeEvent.h"
#include "lightStateTree.h"


class lightTimer
{
private:
    lightStateTree *stateTree;
    Timezone *timezone;
    bool _isOn;
    float _intensity;
    UtilityTicker ticker;

    boolean overrideActive;
    boolean overrideState;
    time_t overrideEnd;
    float overrideIntensity;
    
public:
    lightTimer(timeEvent *events[], int length, Timezone *tz, int refreshRateMs);
    lightTimer();

    void setSchedule(timeEvent *events[], int length);

    void setOverride(int durationMiliseconds, boolean state, float intensity);
    void removeOverride();
    bool getOnStatus();
    float getIntensity();

    
    void evaluateState();

    void loop();
};

#endif