#include "Arduino.h"

#include "communicator.h"


#include <PubSubClient.h>
#include <WiFiClientSecure.h>

/*
    !The maximum message size, including header, is 128 bytes by default. This is configurable via MQTT_MAX_PACKET_SIZE in PubSubClient.h.
*/



void Communicator::loop()
{
    if (!_mqttClient.connected())
    {
        Serial.println("Reconnecting to MQTT");
        reconnect();
    }

    _mqttClient.loop();
}

bool Communicator::send(char *text, char *topic)
{
    return _mqttClient.publish(topic, text);
}

void Communicator::onMqttMessageReceived(char *topic, byte *message, unsigned int length)
{
    Serial.printf("received message on topic: %s\tlen:%d\r\n", topic, strlen(topic));
    Serial.printf("command: len:%d msg:%s\r\n", length, message);
    for (int i = 0; i < _numHandlers; i++)
    {
        Serial.printf("check topic: %s\tlen:%d\r\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));
        if (strcmp(_subscriptionHandlers[i].Topic,topic) == 0)
        {
            Serial.printf("check topic: %s\tlen:%d - matched\r\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));
            //String cmd(message,length)
            Serial.printf("%sexecute callback now\r\n", (char *)message);
            if(_subscriptionHandlers[i].Callback && _subscriptionHandlers[i].Callback != nullptr && _subscriptionHandlers[i].Callback != NULL){
                Serial.println("callback ok");
                bool success =  _subscriptionHandlers[i].Callback((char *)message);
                sendCommandReplay(success);
                Serial.println("callback executed");
            }else{
                Serial.println("callback bad");
            }
            delete message;
            return;
        }
        Serial.printf("check topic: %s\tlen:%d - did not match\r\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));

    }
}

    bool Communicator::sendCommandReplay(bool success){
        char sucessmessage[50];

        if(success){
            sprintf(sucessmessage, replySuccess.c_str(), _timezone->now());

        }else{
            sprintf(sucessmessage, replyFail.c_str(), _timezone->now());
        }
        return send(sucessmessage,_commandReplyTopic);
    }


Communicator::Communicator(char *mqttBrokerIP, uint16_t mqttBrokerPort, char *mqttUsername, char *mqttPassword, char *mqttClientHostname, char *commandReplyTopic, Timezone *timezone)
{
    _mqttClient = PubSubClient(_wifiClientSecure);
    _username = mqttUsername;
    _password = mqttPassword;
    _mqttBrokerIP = mqttBrokerIP;
    _mqttBrokerPort = mqttBrokerPort;
    _hostname = mqttClientHostname;
    _commandReplyTopic = commandReplyTopic;
    _timezone = timezone;



    _mqttClient.setCallback(std::bind(&Communicator::onMqttMessageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _mqttClient.setServer(_mqttBrokerIP, _mqttBrokerPort);
    _mqttClient.connect(_hostname, _username, _password);
    Serial.println("communicator constructor: connect OK");

}

void Communicator::setHandlers(SubscriptionHandler subscriptionHandlers[], int lenHandlers){
    _subscriptionHandlers = subscriptionHandlers;
    _numHandlers = lenHandlers;
    subscribe();
}

void Communicator::subscribe()
{
    Serial.println("communicator subscribe: setcallback now");
    if (_mqttClient.connected())
    {
        for (int i = 0; i < _numHandlers; i++)
        {
            _mqttClient.subscribe(_subscriptionHandlers[i].Topic);
            Serial.print("Subscribe to: ");
            Serial.println(_subscriptionHandlers[i].Topic);
        }
    }
    Serial.println("communicator subscribe: setcallback OK");
}

void Communicator::reconnect()
{

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (_mqttClient.connect(_hostname, _username, _password))
    {
        Serial.println("connected");
        subscribe();
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.print(_mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}