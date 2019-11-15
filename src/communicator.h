#ifndef communicator_h
#define communicator_h

#include "Arduino.h"

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include <functional>

static String replySuccess = "{ \"Success\":true }";
static String replyFail = "{ \"Success\":false }";

//typedef boolean (*callback)(char *cmd);


typedef std::function<boolean(char*)> callback;
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
    char *_commandReplyTopic;

    WiFiClientSecure _wifiClientSecure;
    PubSubClient _mqttClient;

    SubscriptionHandler *_subscriptionHandlers;

    void reconnect();
    void subscribe();

public:
    Communicator(char * mqttBrokerIP, uint16_t mqttBrokerPort, char *mqttUsername, char * mqttPassword, char * mqttClientHostname, char *commandReplyTopic);

    void setHandlers(SubscriptionHandler subscriptionHandlers[], int lenHandlers);
    void onMqttMessageReceived(char *topic, byte *message, unsigned int length);
    bool send(char *text, char *topic);

    bool sendCommandReplay(bool success);

    void loop();
};

#endif