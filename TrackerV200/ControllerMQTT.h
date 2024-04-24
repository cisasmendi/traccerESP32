#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include "ControllerAux.h"
// MQTT details
const char *broker = "64.226.117.238";
const int port = 1883;
const char *topicLed = "GsmClientTest/led";
const char *topicInit = "GsmClientTest/init";
const char *topicLedStatus = "GsmClientTest/ledStatus";

PubSubClient mqtt;


int ledStatus = LOW;
void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();
  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed)
  {
    ledStatus = !ledStatus;
    digitalWrite(PIN_LED_R, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}

//  get mqtt client
PubSubClient& getMQTT()
{
    return mqtt;
}

void initMQTT( TinyGsmClient& client)
{
    mqtt.setClient(client);
    mqtt.setServer(broker, port);
    mqtt.setCallback(mqttCallback);
}
