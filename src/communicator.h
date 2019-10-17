#ifndef communicator_h
#define communicator_h

#include "Arduino.h"

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

typedef void (*callback)(char*);

struct SubscriptionHandler
{
    char *Topic;
    callback Callback;
};

class Communicator
{
private:
    char *_hostname;
    char *_username;
    char *_password;
    char *_mqttBrokerIP;
    uint16_t _mqttBrokerPort;
    int _numHandlers;

    WiFiClientSecure _wifiClientSecure;
    PubSubClient _mqttClient;

    SubscriptionHandler *_subscriptionHandlers;

    void reconnect();
    void subscribe();

public:
    Communicator(char * mqttBrokerIP, uint16_t mqttBrokerPort, char *mqttUsername, char * mqttPassword, char * mqttClientHostname, SubscriptionHandler subscriptionHandlers[], int lenHandlers);
    void onMqttMessageReceived(char *topic, byte *message, unsigned int length);
    bool send(char *text, char *topic);

    void loop();
};

#endif