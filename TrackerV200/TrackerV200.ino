#include "ControllerSim7600.h"
#include "ControllerBluetooth.h"
#include <PubSubClient.h>
#include "ControllerGps.h"
#include "ControllerAux.h"

// MQTT details
const char *broker = "64.226.117.238";
const int port = 1883;
const char *sistem = "bikesistem";
String topicLedStr = String(sistem) + "/led";
String topicInitStr = String(sistem) + "/init";
String topicStatusStr = String(sistem) + "/status";

const char *topicLed = topicLedStr.c_str();
const char *topicInit = topicInitStr.c_str();
const char *topicStatus = topicStatusStr.c_str();

PubSubClient mqtt;

int Status = LOW;

boolean lost = true;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();
  // Only proceed if incoming message's topic matches
   String topiclocal = String(sistem) + "/" + getBluetoothMac();    
  if (String(topic) == topiclocal)
  {
    Status = !Status;
    digitalWrite(PIN_LED_R, Status);
    mqtt.publish(topicStatus, Status ? "1" : "0");
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
  String topic = String(sistem) + "/" + getBluetoothMac();  
   mqtt.subscribe(topic.c_str());
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
  initAux();
  Serial.println("Setup init");
  initSim();
  initMQTT();
  initPas = true;
  Serial.println("Setup done");
}

int count = 0;
unsigned long lastReconnectAttempt = 0;


void loop()
{
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
        mqtt.publish(topic.c_str(), String(count).c_str());
     
        count++;       
      }
    }
  }else{
    lost = true;
   }
  }
}
mqtt.loop();
}
