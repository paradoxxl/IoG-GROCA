#include "Arduino.h"
#include "lightTimer.h"
#include "utilityTicker.h"
#include "lightStateTree.h"

lightTimer::lightTimer(){}
lightTimer::lightTimer(timeEvent *events[], int length, Timezone *tz, int refreshRateMs){
    timezone = tz;
    ticker = UtilityTicker(refreshRateMs);
    stateTree = new lightStateTree(events, length);
}

bool lightTimer::getOnStatus(){
    return _isOn;
}

float lightTimer::getIntensity(){
    if(overrideActive){
        if(now() < overrideEnd){
                return overrideIntensity; 
        }else{
            overrideActive = false;
        }
    }
    return _intensity;
}


void lightTimer::setSchedule(timeEvent *events[], int length){
    stateTree = new lightStateTree(events,length);
}
void lightTimer::setOverride(int durationMiliseconds, boolean state, float intensity){
    overrideActive = true;
    overrideEnd = now()+durationMiliseconds;
    overrideState = state;
    overrideIntensity = intensity;
}

void lightTimer::removeOverride(){
    overrideActive = false;
}

void lightTimer::evaluateState(){
    if(overrideActive){
        if(now() < overrideEnd){
                _isOn = overrideState;
                return;
        }else{
            overrideActive = false;
        }
    }
    _isOn = stateTree->getState(timezone->hour,timezone->minute);
}

void lightTimer::loop(){
    ticker.loop();
    if (ticker.hasTicked()){
        ticker.rst();
        evaluateState();
        Serial.println("lightTimer loop");
    }
}