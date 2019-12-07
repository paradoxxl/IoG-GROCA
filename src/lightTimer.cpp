#include "Arduino.h"
#include "lightTimer.h"
#include "utilityTicker.h"
#include "stateCircularList.h"
#include "lightState.h"
#include <ezTime.h>
#include <ArduinoJson.h>


// XXX: https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods

lightTimer::lightTimer() {}
lightTimer::lightTimer(timeEvent<lightState> *events[], int length, Timezone *tz, int refreshRateMs, Communicator *comm, statusInvokeCallback statusUpdate, uint8_t minIntensity)
{
    _timezone = tz;
    ticker = UtilityTicker(refreshRateMs);
    _stateCircularList = new stateCircularList<lightState>(events, length, lightState(lightState::ON));
    _communicator = comm;
    _commandJsonDocument = new DynamicJsonDocument(256);

    _statusUpdate = statusUpdate;

    _minIntensity = minIntensity;
    _intensity = minIntensity;
}

bool lightTimer::getOnStatus()
{
    return _isOn;
}

uint8_t lightTimer::getIntensity()
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

//TODO: why?
void lightTimer::setSchedule(timeEvent<lightState> *events[], int length)
{
    _stateCircularList = new stateCircularList<lightState>(events, length, lightState(lightState::OFF));
}

void lightTimer::setOverride(int durationMiliseconds, boolean state, uint8_t intensity)
{
    overrideActive = true;
    overrideEnd = now() + durationMiliseconds;
    overrideState = state;
    overrideIntensity = intensity;
}

// removeOverride disables any temporary overrides
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

    uint8_t hour = _timezone->hour();
    uint8_t minute = _timezone->minute();
    Serial.printf("lightTimer getState for %d:%d\r\n", hour, minute);

    lightState s = _stateCircularList->getState(hour,minute);
    Serial.printf("lightTimer isOn?: %d - %d\r\n", s.State(), bool(s));

    _isOn = s.State();
}



//(int durationMiliseconds, boolean state, uint8_t intensity
    boolean lightTimer::cmdOverrideEnable(char *msg){
        Serial.println("lightTimer cmdOverrideEnable");

        const char* tmp = (const char *)msg;
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, tmp);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        return false;
        }

        boolean state = doc["State"];
        uint8_t intensity = doc["Intensity"];
        uint16_t duration = doc["Duration"];

        if(intensity == 0 || duration == 0){
            Serial.println("cmdOverrideEnable - won't set override with 0 intensity or 0 duration");
            return false;
        }

        setOverride(duration,state,intensity);
        _statusUpdate();

        return true;
    }

    boolean lightTimer::cmdDisableOverride(char *msg){
        Serial.println("lightTimer cmdDisableOverride");
        removeOverride();
        _statusUpdate();
        return false;
    }

    boolean lightTimer::cmdIntensity(char *msg){
        Serial.println("lightTimer cmdLightIntensity");

        const char* tmp = (const char *)msg;
        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, tmp);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        return false;
        }

        uint8_t intensity = doc["Intensity"];
        Serial.printf("CmdIntensity:%d\tMinIntensity:%d",intensity,_minIntensity);
        if(intensity >= _minIntensity){
            _intensity = intensity;
            _statusUpdate();
            return true;
        }

        return false;
    }
    boolean lightTimer::cmdPlan(char *msg){
        Serial.println("lightTimer cmdLightPlan");

        const char* tmp = (const char *)msg;
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, tmp);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return false;
        }

        JsonArray schedule = doc.as<JsonArray>();
        _stateCircularList->clear();

        Serial.printf("lightTimer cmdLightPlan - len plan: %d\r\n", schedule.size());
        for(JsonVariant entry: schedule){
            uint16_t usinttime = entry["Time"];
            String sstate = entry["State"];
            lightState state = lightState(sstate);
            // bool bstate = boolean(state);
            Serial.printf("lightTimer cmdLightPlan - time: %d state:%d sstate:%s\r\n", usinttime,bool(state), sstate.c_str());
        }
        for(JsonVariant entry: schedule){
            uint16_t usinttime = entry["Time"];
            String sstate = entry["State"];
            lightState state = lightState(sstate);
            _stateCircularList->insert(usinttime,state);
        }

        _commandJsonDocument->clear();
        return true;
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