#ifndef lightTimer_h
#define lightTimer_h

#include "Arduino.h"
#include <ezTime.h>
#include <ArduinoJson.h>

#include "utilityTicker.h"
#include "timeEvent.h"
#include "stateTree.h"
#include "lightState.h"
#include "communicator.h"

typedef void (*statusInvokeCallback)(void);

class lightTimer
{
private:
    stateTree<lightState> *_stateTree;
    Timezone *_timezone;
    bool _isOn;
    uint8_t _intensity;
    uint8_t _minIntensity;
    UtilityTicker ticker;

    boolean overrideActive;
    boolean overrideState;
    time_t overrideEnd;
    uint8_t overrideIntensity;

    Communicator *_communicator;

    DynamicJsonDocument *_commandJsonDocument;

    statusInvokeCallback _statusUpdate;


    
public:
    lightTimer(timeEvent<lightState> *events[], int length, Timezone *tz, int refreshRateMs, Communicator *comm, statusInvokeCallback statusUpdate, uint8_t minIntensity);
    lightTimer();

    void setSchedule(timeEvent<lightState> *events[], int length);

    void setOverride(int durationMiliseconds, boolean state, uint8_t intensity);
    void removeOverride();
    bool getOnStatus();
    uint8_t getIntensity();

    
    void evaluateState();

    boolean cmdOverrideEnable(char *msg);
    boolean cmdDisableOverride(char *msg);
    boolean cmdIntensity(char *msg);
    boolean cmdPlan(char *msg);

    void loop();
};

#endif