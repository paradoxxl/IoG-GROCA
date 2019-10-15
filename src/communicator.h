#ifndef communicator_h
#define communicator_h

#include "Arduino.h"

typedef void (*callback) (string);

struct SubscriptionHandler{
    String Topic;
    callback Callback;
};

class Communicator{
    private:
        String _hostname;
        String _username;
        String _password;
        String _mqttBrokerIP;
        uint16_t _mqttPort;

        SubscriptionHandler _subscriptionHandlers[];
    public:
    Communicator(String mqttBrokerIP, SubscriptionHandler[] subscriptionHandler);
    void callback(char *topic, byte *message, unsigned int length);

};

#endif