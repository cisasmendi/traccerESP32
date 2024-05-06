//#include "ControllerSim7600.h"
// https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/module/sim7600x/SIM7500_SIM7600_SIM7800%20Series_MQTT_Application%20Note_V2.00.pdf
#include <HardwareSerial.h>

#define MOSFET_SIM 19
#define BAUD 115200
#define RXD2_SIMX 16
#define TXD2_SIMX 17

HardwareSerial &simSerial = Serial2; // Use Serial2 for SIM module

void setup() {
  pinMode(MOSFET_SIM, OUTPUT);
  digitalWrite(MOSFET_SIM, HIGH);
  Serial.begin(BAUD);
  delay(100);
  Serial.println("init....");
  simSerial.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
   delay(5000);
  if (testModemResponse()) {
    if (setupAPN("igprs.claro.com.ar", "", "")) {
      Serial.println("APN configurado correctamente");
      if (testConnect()) {     
        if (connectToServer("elementz/123", "64.226.117.238", 1883)) {
          subscribeToTopic("led/subscribe");                   
        } else {
          Serial.println("Failed to connect to MQTT server.");
        }
      }
    }
  }
}

void loop() {
  // Implementar acciones repetitivas si es necesario
}

bool testModemResponse() {
  return sendCommand("AT", 7000); // Espera hasta 5 segundos por la respuesta "OK"
}

bool testConnect() {
  return sendCommand("AT+CMQTTCONNECT?", 5000); // Espera hasta 5 segundos por la respuesta "OK"
}

bool setupAPN(const String &apn, const String &username, const String &password) {
  if (!username.isEmpty() && !password.isEmpty()) {
    if (!sendCommand("AT+CGAUTH=1,1,\"" + username + "\",\"" + password + "\"", 1000)) return false;
  } else {
    if (!sendCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\"", 1000)) return false;
  }
  return sendCommand("AT+CGACT=1,1", 3000); // Activar el contexto puede tomar más tiempo
}

bool connectToServer(const String &clientId, const String &server, int port) {
  sendCommand("ATE0", 100); // Desactivar eco
  if (!sendCommand("AT+CMQTTSTART", 1000)) return false;
  sendCommand("AT+CMQTTACCQ=0,\"" + clientId + "\"", 1000);
  return sendCommand("AT+CMQTTCONNECT=0,\"tcp://" + server + ":" + port + "\",90,1", 5000);
}

bool subscribeToTopic(const String &topic) {
  int topicLength = topic.length();
  if (!sendCommand("AT+CMQTTSUBTOPIC=0," + String(topicLength) + ",1", 1000)) return false;
  sendCommand(topic, 100);
  return sendCommand("AT+CMQTTSUB=0,4,1,1", 2000);
}

void publishMessage(const String &topic, const String &message) {
  int topicLength = topic.length();
  int messageLength = message.length();
  sendCommand("AT+CMQTTTOPIC=0," + String(topicLength), 100);
  sendCommand(topic, 100);
  sendCommand("AT+CMQTTPAYLOAD=0," + String(messageLength), 100);
  sendCommand(message, 100);
  sendCommand("AT+CMQTTPUB=0,1,60", 2000); // QoS = 1, Retain = 0
}

bool sendCommand(const String &command, unsigned long timeout) {
  simSerial.println(command);
  Serial.println("Sent: " + command); // Debugging output
  return waitForResponse("OK", timeout);
}

bool waitForResponse(const String &expectedResponse, unsigned long timeout) {
  unsigned long startTime = millis();
  String response;
  while (millis() - startTime < timeout) {
    if (simSerial.available()) {
      response += char(simSerial.read());
      if (response.indexOf(expectedResponse) != -1) {
        Serial.print("Received: " + response); // Debugging output
        return true;
      }
    }
  }
  Serial.println("Timeout or unexpected response: " + response); // Debugging output
  return false;
}
