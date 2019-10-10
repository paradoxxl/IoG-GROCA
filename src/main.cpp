#include <Arduino.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <PubSubClient.h>

#include <ezTime.h>

#include "utilityTicker.h"
#include "lightTimer.h"
#include "lightState.h"


#define ONBOARDLED 5 // Built in LED on ESP-12/ESP-07
#define SHOW_TIME_PERIOD 5000


const String hostname = "groca1";

WebServer Server;
AutoConnect Portal(Server);

//MQTT
const char *mqtt_server = "192.168.0.9";
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String commandTopic = ("cabinet/" + hostname + "/command");
String statusTopic = ("cabinet/" + hostname + "/status");

//StatusJSON
DynamicJsonDocument doc(1024);

UtilityTicker statusUpdateTimer;
Timezone timeZone;

lightTimer lightscheduler;

void rootPage()
{
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}



void sendStatusUpdate()
{
  statusUpdateTimer.rst();

  Serial.print("create status json ");
  Serial.println(timeZone.dateTime());
  doc.clear();
  doc["timestamp"] = timeZone.now();
  doc["ledState"] = lightscheduler.getOnStatus();

  Serial.println("serialize status json");
  char res[128];
  int n = serializeJson(doc, res);
  if (n > 0)
  {
    Serial.println("res ok");
    Serial.println(res);
    if (mqttClient.publish(statusTopic.c_str(), res))
    {
      Serial.println("Sent status successfully");
    }
    else
    {
      Serial.println("Could not send status");
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic).equals(commandTopic))
  {
    Serial.print("Changing output to ");
    lightState tmp = lightState(messageTemp);
    Serial.println(tmp);
  }
  sendStatusUpdate();
}



void setup()
{
  Serial.begin(115200);
  Serial.println();
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);


  statusUpdateTimer = UtilityTicker(1000);


  Server.on("/", rootPage);
  if (Portal.begin())
  {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println("wifiFirstConnected: true");
  }

  mqttClient.setServer(mqtt_server, 8883);
  mqttClient.setCallback(callback);


  timeZone = new Timezone();
  timeZone.setLocation("CEST");
  waitForSync();
  Serial.println("CEST synced time: " + timeZone.dateTime());

  setInterval(10);
	setDebug(INFO);


  lightscheduler = lightTimer(nullptr,1,&timeZone, 2000);
}



void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("groca1", "groca1", "T1kDKCqNzc7QQa4Lp9B4"))
    {
      Serial.println("connected");
      // Subscribe
      mqttClient.subscribe(commandTopic.c_str());
      Serial.print("Subscribe to: ");
      Serial.println(commandTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop()
{
  Portal.handleClient();
  if (!mqttClient.connected())
  {
    reconnect();
  }

  mqttClient.loop();
  

  statusUpdateTimer.loop();
  if (statusUpdateTimer.hasTicked()){
    statusUpdateTimer.rst();
    sendStatusUpdate();
  }

  if(lightscheduler.getOnStatus())
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }

  lightscheduler.loop();
  events();
}
