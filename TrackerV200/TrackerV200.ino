#include "ControllerSim7600.h"
#include "ControllerBluetooth.h"
#include <PubSubClient.h>
#include "ControllerGps.h"
#include "ControllerAux.h"
#include "ControllerWiFi.h"


// MQTT details
const char *broker = "64.226.117.238";
const int port = 1883;
const char *sistem = "bikesistem";
String topicStr = String(sistem) + "/"+getBluetoothMac();
String topicInitStr = String(sistem) + "/init";
String topicStatusStr = String(sistem) + "/"+getBluetoothMac()+"/st"; //bikesistem/8:d1:f9:d0:31:ee/st

const char *topicLed = topicStr.c_str();
const char *topicInit = topicInitStr.c_str();
const char *topicStatus = topicStatusStr.c_str();

PubSubClient mqtt;

int Status = LOW;

boolean lost = true;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{       
  // Only proceed if incoming message's topic matches    
  if (String(topic) == topicStatusStr)
  {
      Serial.print(F("Message arrived ["));
      Serial.print(topic);
      Serial.print(F("]: "));
      Serial.write(payload, len);
      Serial.print(F("\n"));
      Status = !Status;
      digitalWrite(PIN_LED_R, Status);
   // mqtt.publish(topicStatus, Status ? "1" : "0");
  }
}

String miTopic(){
  return String(sistem) + "/" + getBluetoothMac();
}

boolean mqttConnect()
{
  //Serial.print("Connecting to ");
  //Serial.print(broker);
  // Connect to MQTT Broker
  boolean status = mqtt.connect(sistem);
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
  if (!status)
  {
    //Serial.println(" fail");    
    return false;
  }
  //Serial.println(" success");
  if(lost){
   String mensaje_init = "re-started: " + getBluetoothMac();
   mqtt.publish(topicInit,mensaje_init.c_str());  
   mqtt.subscribe(topicStatus);
   lost = false;
  }
  return mqtt.connected();
}

void initMQTT()
{
  mqtt.setClient(client);
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback);
}

boolean initPas = false;
boolean reconect = false;

void setup()
{
  Serial.begin(115200);
  initBluetooth();
  initAux();
  Serial.println(F("Setup init"));
  initWiFi("asdasd","asdasd");
  initGps();
  initSim();
  initPas = true;
  Serial.println(F("Setup done"));
}

int count = 0;
unsigned long lastReconnectAttempt = 0;

void logicSIM7X_GPS(){
    unsigned long currentMillis = millis();
  if (currentMillis - lastReconnectAttempt > 5000)  {
   // checkModemStatus();
    lastReconnectAttempt = currentMillis;  
  if(initPas){  
  if (testModem())  {
    //  Serial.println("modem on");
    if (testNetwork())
    {
      //   Serial.println("Network on");
      if (mqttConnect())
      {
        //   Serial.println("MQTT is connected");    
        String topic = String(sistem) + "/" + getBluetoothMac();   
        String mensaje = "{\"id\":"+String(count)+ getGpsData(); 
        mqtt.publish(topic.c_str(), mensaje.c_str());     
        count++;       
      }
    }
  }else{
    lost = true;
   }
  }
}
}
void loop(){  
  logicSIM7X_GPS();
  mqtt.loop();
  loopBluetooth();  
}
