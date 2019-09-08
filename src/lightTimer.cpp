#include "Arduino.h"
#include "lightTimer.h"
#include "utilityTicker.h"

lightTimer::lightTimer(){}
lightTimer::lightTimer(timeEvent events[], int length, Timezone *tz, int refreshRateMs){
    sortEvents(events, length);
    timezone = tz;
    ticker = UtilityTicker(refreshRateMs);
}

boolean lightTimer::isOn(){
    return _isOn;
}

void lightTimer::sortEvents(timeEvent unsortedEvents[], int length){
    
}

void lightTimer::evaluateState(){
    _isOn = !_isOn;
}

void lightTimer::loop(){
    ticker.loop();
    if (ticker.hasTicked()){
        ticker.rst();
        evaluateState();
        Serial.println("lightTimer loop");
    }
}