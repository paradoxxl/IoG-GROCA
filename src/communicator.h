#ifndef communicator_h
#define communicator_h

#include "Arduino.h"

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

typedef void (*callback)(String);

struct SubscriptionHandler
{
    String Topic;
    callback Callback;
};

class Communicator
{
private:
    String _hostname;
    String _username;
    String _password;
    String _mqttBrokerIP;
    uint16_t _mqttBrokerPort;
    int _numHandlers;

    WiFiClientSecure _wifiClientSecure;
    PubSubClient _mqttClient;

    SubscriptionHandler *_subscriptionHandlers;

    void reconnect();
    void subscribe();

public:
    Communicator(String mqttBrokerIP, uint16_t mqttBrokerPort, String mqttUsername, String mqttPassword, String mqttClientHostname, SubscriptionHandler subscriptionHandlers[], int lenHandlers);
    void onMqttMessageReceived(char *topic, byte *message, unsigned int length);
    void send(char *text, char *topic);

    void loop();
};

#endif