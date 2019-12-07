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
#include "communicator.h"

#include <functional>


#define ONBOARDLED 5 // Built in LED on ESP-12/ESP-07
#define SHOW_TIME_PERIOD 5000

const String hostname = "groca1";

WebServer Server;
AutoConnect Portal(Server);

//MQTT
Communicator *mqttComm = nullptr;
const char *mqtt_server = "192.168.0.9";
const char *mqtt_username = "groca1";
const char *mqtt_password = "T1kDKCqNzc7QQa4Lp9B4";
const char *mqtt_id = "groca1";
const uint16_t mqtt_Port = 8883;

String statusTopic = ("cabinet/" + hostname + "/status");
String commandReplyTopic = ("cabinet/" + hostname + "/commandReply");
String wakeupTopic = ("cabinet/" + hostname + "/wakeup");

String cmdLighOverrideEnableTopic = ("cabinet/" + hostname + "/command/light/override/enable");
String cmdLighhOverrideDisableTopic = ("cabinet/" + hostname + "/command/light/override/disable");
String cmdLightIntensityTopic = ("cabinet/" + hostname + "/command/light/intensity");
String cmdLightPlanTopic = ("cabinet/" + hostname + "/command/light/plan");


const uint8_t minIntensity = 153;  // equals to 60% as the LED driver should not go below that


SubscriptionHandler *mqttHandlers;

//StatusJSON
DynamicJsonDocument statusJsonDoc(128);

UtilityTicker statusUpdateTimer;
Timezone timeZone;

lightTimer *lightscheduler;

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
  statusJsonDoc.clear();
  statusJsonDoc["timestamp"] = timeZone.now();
  statusJsonDoc["ledState"] = lightscheduler->getOnStatus();
  statusJsonDoc["ledIntensity"] = lightscheduler->getIntensity();


  Serial.println("serialize status json");
  char res[128];
  int n = serializeJson(statusJsonDoc, res);
  if (n > 0)
  {
    Serial.println("res ok");
    Serial.println(res);
    if (mqttComm->send(res, &statusTopic[0]))
    {
      Serial.println("Sent status successfully");
    }
    else
    {
      Serial.println("Could not send status");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);

  statusUpdateTimer = UtilityTicker(10000);

  Server.on("/", rootPage);
  if (Portal.begin())
  {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println("wifiFirstConnected: true");
  }

  Serial.println("subscription handlers defined");

  timeZone.setLocation("Europe/Zurich");
  timeZone.setCache(0);

  waitForSync();
  Serial.println("Europe/Zurich synced time: " + timeZone.dateTime());

  setInterval(10);
  setDebug(INFO);
  

  mqttComm = new Communicator((char *)mqtt_server, mqtt_Port, (char *)mqtt_username, (char *)mqtt_password, (char *)mqtt_id, &commandReplyTopic[0],&timeZone);

  lightscheduler = new lightTimer(nullptr, 0, &timeZone, 2000, mqttComm, sendStatusUpdate, minIntensity);
  Serial.println("light scheduler created");

  mqttHandlers = new SubscriptionHandler[4]{
    {&cmdLighOverrideEnableTopic[0], std::bind(&lightTimer::cmdOverrideEnable, lightscheduler, std::placeholders::_1)},
    {&cmdLighhOverrideDisableTopic[0], std::bind(&lightTimer::cmdDisableOverride, lightscheduler, std::placeholders::_1)},
    {&cmdLightIntensityTopic[0], std::bind(&lightTimer::cmdIntensity, lightscheduler, std::placeholders::_1)},
    {&cmdLightPlanTopic[0], std::bind(&lightTimer::cmdPlan, lightscheduler, std::placeholders::_1)}
    };

  mqttComm->setHandlers(mqttHandlers, 4);

  Serial.println("mqtt comm created");



char wakeupMessage[50];
sprintf(wakeupMessage, "{ \"Timestamp\":%lu }", timeZone.now());
mqttComm->send(wakeupMessage, &wakeupTopic[0]);
}

void loop()
{
  Portal.handleClient();
  mqttComm->loop();

  statusUpdateTimer.loop();
  if (statusUpdateTimer.hasTicked())
  {
    sendStatusUpdate();
  }

  if (lightscheduler->getOnStatus())
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  lightscheduler->loop();
  events();
}
