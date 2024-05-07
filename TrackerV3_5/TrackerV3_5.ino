#include <Arduino.h>
// Variables para el manejo de las tareas
TaskHandle_t Task1;
TaskHandle_t Task2;

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
boolean alarmStatus = false;
boolean openTransmission = false;

int count = 0;
unsigned long lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len) {
    if (strcmp(topic, topicStatusStr.c_str()) == 0) {
        // Creamos un buffer para el contenido que incluya espacio para el carácter nulo
        char content[len + 1];
        memcpy(content, payload, len);
        content[len] = '\0'; // Asegurar el carácter nulo al final para formar una cadena C válida

        // Preparar el topic de respuesta
        String topicResp = String(topicStatusStr) + "/AT_RESP";

        // Procesar diferentes tipos de mensajes
        if (strncmp(content, "AT", 2) == 0) {
            String response = handleCommands(content);
            Serial.print(F("Response to: "));
            Serial.println(response);
            mqtt.publish(topicResp.c_str(), response.c_str());
        } else if (strcmp(content, "alarm:1") == 0) {
            alarmStatus = true;
            mqtt.publish(topicResp.c_str(), "alarm on resp");
        } else if (strcmp(content, "alarm:0") == 0) {
            alarmStatus = false;
            mqtt.publish(topicResp.c_str(), "alarm off resp");
        } else if (strcmp(content, "transm:1") == 0) {
            openTransmission = true;
            mqtt.publish(topicResp.c_str(), "transm on resp");
        } else if (strcmp(content, "transm:0") == 0) {
            openTransmission = false;
            mqtt.publish(topicResp.c_str(), "transm off resp");
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
    count = 0;
    String mensaje_close = "close: " + getBluetoothMac() + "{\"id\":" + String(count) + getGpsData();
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
    //  Serial.println(F(" fail"));
      return false;
    }
  //  Serial.println(F(" success"));
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
    xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 1); 
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1); 
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
         // Serial.println("modem on");
          if (testNetwork())
          {
          //  Serial.println("Network on");
            if (mqttConnect())
            {
           //   Serial.println("MQTT is connected");
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

  void Task2code(void *parameter)
  {
    for (;;)
    {
      Serial.print(F("status alarm: "));
      Serial.println(alarmStatus);
      if(alarmStatus)
         runAlarm();
    }
  }

  void secuenceTransmission()
  {
    digitalWrite(ON_OFF_GPS, HIGH); // encender el módulo GPS
    digitalWrite(MOSFET_SIM, HIGH); // encender el módulo SIM
    // Llamar a función de lógica para el GPS y enviar mensaje por mqtt
    logicSIM7X_GPS();
    // Iniciar parpadeo del LED
    startBlink();
    // Procesar loop de MQTT para mantener la comunicación
    mqtt.loop();
  }

  void secuenceStop()
  {
    // Detener todas las operaciones después de 5 minutos sin detectar HIGH
    sendClose();
    digitalWrite(ON_OFF_GPS, LOW); // Apagar el módulo GPS
    digitalWrite(MOSFET_SIM, LOW); // Apagar el módulo SIM
    stopBlink();                   // Detener el parpadeo del LED
  }

  bool transmite = false;
  void loop()
  {
    static unsigned long lastTimeSensorWasHigh = 0; // Guardar última vez que el sensor estuvo en HIGH
    static bool isTimeCounting = false;             // Indicador de si estamos en periodo de gracia
    int hallValue = digitalRead(PIN_SENSOR_HALL);

    if (hallValue == HIGH)
    {
      // Serial.println(F("no esta el candado"));
      lastTimeSensorWasHigh = millis();
      isTimeCounting = true;
      transmite = true;
    }
    else
    {
      //  Serial.println(F("esta el candado"));
      if (isTimeCounting && (millis() - lastTimeSensorWasHigh > 180000))
      {
        // Si pasan más de 3 minutos desde la última vez que el sensor fue HIGH
        isTimeCounting = false; // Detener el conteo
        transmite = false;      // Detener transmisión
      }
    }
    Serial.print(F("status trans: "));
      Serial.println(openTransmission);

    if (transmite || openTransmission)    
      secuenceTransmission();    
    else    
      secuenceStop();
    
  }
