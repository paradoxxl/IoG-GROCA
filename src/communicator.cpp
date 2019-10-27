#include "Arduino.h"

#include "communicator.h"

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

void Communicator::loop()
{
    if (!_mqttClient.connected())
    {
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
    Serial.printf("received message on topic: %s\tlen:%d\n", topic, strlen(topic));
    Serial.printf("command: len:%d msg:%s\n", length, message);
    for (int i = 0; i < _numHandlers; i++)
    {
        Serial.printf("check topic: %s\tlen:%d\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));
        if (_subscriptionHandlers[i].Topic == topic)
        {
            Serial.printf("check topic: %s\tlen:%d - matched\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));
            //String cmd(message,length)
            Serial.printf("%sexecute callback now\n", (char *)message);
            if(_subscriptionHandlers[i].Callback && _subscriptionHandlers[i].Callback != nullptr && _subscriptionHandlers[i].Callback != NULL){
                Serial.println("callback ok");
                _subscriptionHandlers[i].Callback();
                Serial.println("callback executed");
            }else{
                Serial.println("callback bad");
            }
            return;
        }
        Serial.printf("check topic: %s\tlen:%d - did not match\n", _subscriptionHandlers[i].Topic, strlen(_subscriptionHandlers[i].Topic));

    }
}

Communicator::Communicator(char *mqttBrokerIP, uint16_t mqttBrokerPort, char *mqttUsername, char *mqttPassword, char *mqttClientHostname, SubscriptionHandler *subscriptionHandlers, int lenHandlers)
{
    _mqttClient = PubSubClient(_wifiClientSecure);
    _username = mqttUsername;
    _password = mqttPassword;
    _mqttBrokerIP = mqttBrokerIP;
    _mqttBrokerPort = mqttBrokerPort;
    _hostname = mqttClientHostname;

    _subscriptionHandlers = subscriptionHandlers;
    _numHandlers = lenHandlers;

    _mqttClient.setCallback(std::bind(&Communicator::onMqttMessageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _mqttClient.setServer(_mqttBrokerIP, _mqttBrokerPort);
    _mqttClient.connect(_hostname, _username, _password);
    Serial.println("communicator constructor: connect OK");

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