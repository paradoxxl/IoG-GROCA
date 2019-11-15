#ifndef communicator_h
#define communicator_h

#include "Arduino.h"

#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ezTime.h>
#include <functional>

static String replySuccess = "{ \"Timestamp\":%lu, \"Success\":true }";
static String replyFail =  "{ \"Timestamp\":%lu, \"Success\":false }";
struct SubscriptionHandler
{
    char *Topic;
    std::function<boolean(char*)> Callback;
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
    char *_commandReplyTopic;
    Timezone *_timezone;

    WiFiClientSecure _wifiClientSecure;
    PubSubClient _mqttClient;

    SubscriptionHandler *_subscriptionHandlers;

    void reconnect();
    void subscribe();

public:
    Communicator(char * mqttBrokerIP, uint16_t mqttBrokerPort, char *mqttUsername, char * mqttPassword, char * mqttClientHostname, char *commandReplyTopic, Timezone *timezone);

    void setHandlers(SubscriptionHandler subscriptionHandlers[], int lenHandlers);
    void onMqttMessageReceived(char *topic, byte *message, unsigned int length);
    bool send(char *text, char *topic);

    bool sendCommandReplay(bool success);

    void loop();
};

#endif