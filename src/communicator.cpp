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

void Communicator::send(char *text, char *topic)
{
}

void Communicator::onMqttMessageReceived(char *topic, byte *message, unsigned int length)
{
}

Communicator::Communicator(String mqttBrokerIP, uint16_t mqttBrokerPort, String mqttUsername, String mqttPassword, String mqttClientHostname, SubscriptionHandler *subscriptionHandlers, int lenHandlers)
{
    _mqttClient = PubSubClient(_wifiClientSecure);
    _username = mqttUsername;
    _password = mqttPassword;
    _mqttBrokerIP = mqttBrokerIP;
    _mqttBrokerPort = mqttBrokerPort;

    _subscriptionHandlers = subscriptionHandlers;
    _numHandlers = lenHandlers;

    _mqttClient.setCallback(this.onMqttMessageReceived);
    _mqttClient.setServer(_mqttBrokerIP.c_str(), _mqttBrokerPort);
    _mqttClient.connect(_hostname.c_str(), _username.c_str(), _password.c_str());
    subscribe();
}

void Communicator::subscribe()
{
    if (_mqttClient.connected())
    {
        for (int i = 0; i < _numHandlers; i++)
        {
            _mqttClient.subscribe(_subscriptionHandlers[i].Topic.c_str());
            Serial.print("Subscribe to: ");
            Serial.println(_subscriptionHandlers[i].Topic);
        }
    }
}

void Communicator::reconnect()
{

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (_mqttClient.connect(_hostname.c_str(), _username.c_str(), _password.c_str()))
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