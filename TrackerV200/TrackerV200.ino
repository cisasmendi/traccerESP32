#include <Arduino.h>
// Variables para el manejo de las tareas
TaskHandle_t Task1;
//TaskHandle_t Task2;


#include "ControllerSim7600.h"
#include "ControllerBluetooth.h"
#include <PubSubClient.h>
#include "ControllerGps.h"
#include "ControllerAux.h"

// #include "ControllerWiFi.h"

String broker;
String topicSistem;
int port;
char *broker1 = nullptr;
// MQTT details

/*
const char *broker = "64.226.117.238";
const int port = 1883;
const char *sistem = "bikesistem";
String topicStr = String(sistem) + "/"+getBluetoothMac();
String topicInitStr = String(sistem) + "/init";
 = String(sistem) + "/"+getBluetoothMac()+"/st"; //bikesistem/8:d1:f9:d0:31:ee/st

const char *topicLed = topicStr.c_str();
const char *topicInit = topicInitStr.c_str();
const char *topicStatus = topicStatusStr.c_str();
*/
String topicStatusStr = "";
PubSubClient mqtt;
int Status = LOW;
boolean lost = true;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  // Only proceed if incoming message's topic matches
  if (String(topic) == topicStatusStr)
  {
    String content = "";
    for (size_t i = 0; i < len; i++)
    {
        content.concat((char)payload[i]);
    }
    
    // analiza el mensaje si es un comando AT
    if (content.startsWith("AT"))
    {        
      String response = handleCommands(content); 
      String  topicResp= topicStatusStr+"/AT_RESP";
      Serial.print(F("resposne to: "));
      Serial.print(response);
      mqtt.publish(topicResp.c_str(), response.c_str());     
      return;
    }
  }
}

boolean mqttConnect()
{
  Serial.print(F("Connecting to: "));
  Serial.print(broker);
  // Connect to MQTT Broker
  String miTopic = String(topicSistem) + "/" + getBluetoothMac();
  boolean status = mqtt.connect(miTopic.c_str());
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
  if (!status)
  {
    Serial.println(F(" fail"));
    return false;
  }
  Serial.println(F(" success"));
  if (lost)
  {
    String mensaje_init = "re-started: " + getBluetoothMac();
    String topicInit = String(topicSistem) + "/init";
    topicStatusStr = String(topicSistem) + "/" + getBluetoothMac() + "/st";
    mqtt.publish(topicInit.c_str(), mensaje_init.c_str());
    mqtt.subscribe(topicStatusStr.c_str());
    lost = false;
  }
  return mqtt.connected();
}

void initMQTT(String broker, int port, TinyGsmClient &cliente)
{

  if (broker1 != nullptr)
  {
    delete[] broker1;
  }
  // Asignar memoria para broker1 incluyendo espacio para el carácter nulo
  broker1 = new char[broker.length() + 1];
  // Copiar el contenido del String al char*
  strcpy(broker1, broker.c_str());
  mqtt.setClient(cliente);
  mqtt.setServer(broker1, port);
  mqtt.setCallback(mqttCallback);
}

boolean initPas = false;
boolean reconect = false;

void setup()
{
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 1); // Tarea para el Core 0
//  xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1); // Tarea para el Core 1

  String apn;
  String gprsUser;
  String gprsPass;

  Serial.begin(115200);
  initBluetooth();
  delay(1000);

  getConfigApn(apn, gprsUser, gprsPass);
  getConfigMqtt(broker, port, topicSistem);
  initMQTT(broker, port, client);
  initAux();
  Serial.println(F("Setup init"));
  // initWiFi("asdasd","asdasd");
  initGps();
  initSim(apn, gprsUser, gprsPass);
  initPas = true;
  Serial.println(F("Setup done"));
}

int count = 0;
unsigned long lastReconnectAttempt = 0;

void logicSIM7X_GPS()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastReconnectAttempt > 30000)
  {
    // checkModemStatus();
    lastReconnectAttempt = currentMillis;
    if (initPas)
    {
      if (testModem())
      {
        Serial.println("modem on");
        if (testNetwork())
        {
          Serial.println("Network on");
          if (mqttConnect())
          {
            Serial.println("MQTT is connected");
            String miTopic = String(topicSistem) + "/" + getBluetoothMac();
            String mensaje = "{\"id\":" + String(count) + getGpsData();
            mqtt.publish(miTopic.c_str(), mensaje.c_str());
            count++;
          }
        }
      }
      else
      {
        lost = true;
      }
    }
  }
}

void Task1code(void * parameter) {
  for (;;) {
    loopBluetooth();   
  }
}


void loop()
{
  //loopBluetooth();
  logicSIM7X_GPS();
  mqtt.loop();
}

