#include "Arduino.h"
#include "lightTimer.h"
#include "utilityTicker.h"
#include "stateTree.h"
#include "lightState.h"

// XXX: https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods

lightTimer::lightTimer(){}
lightTimer::lightTimer(timeEvent<lightState> *events[], int length, Timezone *tz, int refreshRateMs){
    timezone = tz;
    ticker = UtilityTicker(refreshRateMs);
    _stateTree = new stateTree<lightState>(events, length, new lightState(lightState::ON));
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


void lightTimer::setSchedule(timeEvent<lightState> *events[], int length){
    _stateTree = new stateTree<lightState>(events,length, new lightState(lightState::OFF));
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
    //timezone->hour,timezone->minute
    lightState *s = _stateTree->getState(0,0);
    _isOn = s->State();
}

void lightTimer::loop(){
    ticker.loop();
    if (ticker.hasTicked()){
        ticker.rst();
        evaluateState();
        Serial.println("lightTimer loop");
    }
}