#include <Arduino.h>
// Variables para el manejo de las tareas
TaskHandle_t Task1;
// TaskHandle_t Task2;

#include "ControllerSim7600.h"
#include "ControllerBluetooth.h"
#include <PubSubClient.h>
#include "ControllerGps.h"
#include "ControllerAux.h"

String broker;
String topicSistem;
int port;
char *broker1 = nullptr;
String topicStatusStr = "";
PubSubClient mqtt;
int Status = LOW;
boolean lost = true;
boolean initPas = false;
boolean reconect = false;
int count = 0;
unsigned long lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
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
      String topicResp = topicStatusStr + "/AT_RESP";
      Serial.print(F("resposne to: "));
      Serial.print(response);
      mqtt.publish(topicResp.c_str(), response.c_str());
      return;
    }
  }
}

void sendReconect()
{
  String mensaje_init = "re-start: " + getBluetoothMac() + "{\"id\":" + String(count) + getGpsData();
  String topicInit = String(topicSistem) + "/init";
  mqtt.publish(topicInit.c_str(), mensaje_init.c_str());
}

void sendPosition()
{
  String miTopic = String(topicSistem) + "/" + getBluetoothMac();
  String mensaje = "{\"id\":" + String(count) + getGpsData();
  mqtt.publish(miTopic.c_str(), mensaje.c_str());
  count++;
}

void sendClose()
{ 
  count=0;
  String mensaje_close = "close: " + getBluetoothMac()+ "{\"id\":" + String(count) + getGpsData();
  String topicInit = String(topicSistem) + "/close";
  mqtt.publish(topicInit.c_str(), mensaje_close.c_str());
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
    sendReconect();
    topicStatusStr = String(topicSistem) + "/" + getBluetoothMac() + "/st";
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

void setup()
{
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 1); // Tarea para el Core 0
  String apn;
  String gprsUser;
  String gprsPass;
  Serial.begin(115200);
  initAux();
  initBluetooth(ON_OFF_GPS, MOSFET_SIM);
  delay(1000);
  getConfigApn(apn, gprsUser, gprsPass);
  getConfigMqtt(broker, port, topicSistem);
  initMQTT(broker, port, client);

  Serial.println(F("Setup init"));
  initGps();
  initSim(apn, gprsUser, gprsPass);
  initPas = true;
  Serial.println(F("Setup done"));
}


void logicSIM7X_GPS()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastReconnectAttempt > 5000)
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
            sendPosition();
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

void Task1code(void *parameter)
{
  for (;;)
  {
    loopBluetooth();
    loopAux();
  }
}

void loop()
{
  static unsigned long lastTimeSensorWasHigh = 0; // Guardar última vez que el sensor estuvo en HIGH
  static bool isTimeCounting = false;             // Indicador de si estamos en periodo de gracia

  int hallValue = digitalRead(PIN_SENSOR_HALL);

  if (hallValue == HIGH)
  {
    Serial.println(F("no esta el candado"));
    digitalWrite(ON_OFF_GPS, HIGH); // Apagar el módulo GPS
    digitalWrite(MOSFET_SIM, HIGH); // Apagar el módulo SIM
    // Restablecer el contador cada vez que el sensor esté en HIGH
    lastTimeSensorWasHigh = millis();
    isTimeCounting = false; // Reiniciar la cuenta porque el sensor está en HIGH
    // Llamar a función de lógica para el GPS
    logicSIM7X_GPS();
    // Iniciar parpadeo del LED
    startBlink();
    // Procesar loop de MQTT para mantener la comunicación
    mqtt.loop();
  }
  else if (!isTimeCounting)
  {
    Serial.println(F("esta el candado"));
    // Comenzar a contar tiempo desde la última detección HIGH
    isTimeCounting = true;
  }

  if (isTimeCounting && millis() - lastTimeSensorWasHigh > 180000)
  { // 300000 ms = 5 minutos  
    // Detener todas las operaciones después de 5 minutos sin detectar HIGH
    sendClose();
    digitalWrite(ON_OFF_GPS, LOW); // Apagar el módulo GPS
    digitalWrite(MOSFET_SIM, LOW); // Apagar el módulo SIM
    stopBlink();                   // Detener el parpadeo del LED
    isTimeCounting = false;        // Restablecer el contador de tiempo
  }
  else
  {
    // Continuar operaciones mientras se está en periodo de gracia
    Serial.println(F("tiempo de gracia"));
    startBlink();    
    mqtt.loop();
    logicSIM7X_GPS();
  }
}
