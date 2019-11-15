#include "Arduino.h"
#include "lightTimer.h"
#include "utilityTicker.h"
#include "stateTree.h"
#include "lightState.h"
#include <ezTime.h>

// XXX: https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods

lightTimer::lightTimer() {}
lightTimer::lightTimer(timeEvent<lightState> *events[], int length, Timezone *tz, int refreshRateMs)
{
    _timezone = tz;
    ticker = UtilityTicker(refreshRateMs);
    _stateTree = new stateTree<lightState>(events, length, new lightState(lightState::ON));
}

bool lightTimer::getOnStatus()
{
    return _isOn;
}

float lightTimer::getIntensity()
{
    if (overrideActive)
    {
        if (now() < overrideEnd)
        {
            return overrideIntensity;
        }
        else
        {
            overrideActive = false;
        }
    }
    return _intensity;
}

void lightTimer::setSchedule(timeEvent<lightState> *events[], int length)
{
    _stateTree = new stateTree<lightState>(events, length, new lightState(lightState::OFF));
}
void lightTimer::setOverride(int durationMiliseconds, boolean state, float intensity)
{
    overrideActive = true;
    overrideEnd = now() + durationMiliseconds;
    overrideState = state;
    overrideIntensity = intensity;
}

void lightTimer::removeOverride()
{
    overrideActive = false;
}

void lightTimer::evaluateState()
{
    Serial.println("lightTimer evaluate state");

    if (overrideActive)
    {
        if (now() < overrideEnd)
        {
            Serial.println("lightTimer override active");

            _isOn = overrideState;
            return;
        }
        else
        {
            Serial.println("lightTimer override deactivated now");

            overrideActive = false;
        }
    }
    Serial.println("lightTimer getState");

    lightState *s = _stateTree->getState(_timezone->hour(), _timezone->minute());
    Serial.println("lightTimer getState ok");

    Serial.println("lightTimer ison: ");

    _isOn = s->State();
}



    boolean cmdOverrideEnable(char *msg){
        Serial.println("lightTimer cmdOverrideEnable");
        return false;
    }
    boolean cmdDisableOverride(char *msg){
        Serial.println("lightTimer cmdDisableOverride");
        return false;
    }
    boolean cmdIntensity(char *msg){
        Serial.println("lightTimer cmdLightIntensity");
        return false;
    }
    boolean cmdPlan(char *msg){
        Serial.println("lightTimer cmdLightPlan");
        return false;
    }



void lightTimer::loop()
{
    ticker.loop();
    if (ticker.hasTicked())
    {
        Serial.println("lightTimer loop start");

        ticker.rst();
        evaluateState();
        Serial.println("lightTimer loop end");
    }
}