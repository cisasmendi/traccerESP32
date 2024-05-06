#include <HardwareSerial.h>

#define MOSFET_SIM 19
#define BAUD 115200
#define RXD2_SIMX 16
#define TXD2_SIMX 17

HardwareSerial &simSerial = Serial2; // Use Serial2 for SIM module

void setup()
{ 
  pinMode(MOSFET_SIM, OUTPUT);
  digitalWrite(MOSFET_SIM, HIGH);
  Serial.begin(BAUD);
  delay(100);
  simSerial.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
  
}

int counter = 0;
void loop()
{
 if (testModemResponse())
  {
    if(setupAPN("igprs.claro.com.ar", "", "")){
      Serial.println("APN configurado correctamente");
      if(testConect()){
        String message = "Count: " + String(counter++); 
        publishMessage("led/count", message); 
      }else{       
        if (connectToServer("elementz/123", "64.226.117.238", 1883)){
          subscribeToTopic("led/subscribe");                   
        }else{
          Serial.println("Failed to connect to MQTT server.");
        }
      }
    }
  }
}

bool testModemResponse()
{
  simSerial.println("AT");  
  return waitForResponse("OK", 5000); // Espera hasta 5 segundos por la respuesta "OK"
}

bool testConect()
{
  simSerial.println("AT+CMQTTCONNECT?");
  return waitForResponse("OK", 5000); // Espera hasta 5 segundos por la respuesta "OK"
}

bool setupAPN(const String &apn, const String &username, const String &password)
{
  
  if (username.length() > 0 && password.length() > 0)
  {
    simSerial.println("AT+CGAUTH=1,1,\"" + username + "\",\"" + password + "\"");
    if (!waitForResponse("OK", 50))
      return false;
  }else{
    simSerial.println("AT+CGDCONT=1,\"IP\",\"" + apn + "\"");
    if (!waitForResponse("OK", 50))
      return false;
  }

  simSerial.println("AT+CGACT=1,1");
  return waitForResponse("OK", 50); // Puede tomar más tiempo activar el contexto
}

bool connectToServer(const String &clientId, const String &server, int port)
{
  simSerial.println("ATE0");
  delay(20);
  simSerial.println("AT+CMQTTSTART");
  waitForResponse("OK", 50);
  simSerial.println("AT+CMQTTACCQ=0,\"" + clientId + "\"");
  delay(20);
  simSerial.println("AT+CMQTTCONNECT=0,\"tcp://" + server + ":" + String(port) + "\",90,1");
  return waitForResponse("OK", 50); // Asumiendo que OK indica éxito aquí
}

bool subscribeToTopic(const String &topic)
{
  int topicLength = topic.length();
  simSerial.println("AT+CMQTTSUBTOPIC=0," + String(topicLength) + ",1");
  delay(20);
  simSerial.println(topic);
  delay(20);
  simSerial.println("AT+CMQTTSUB=0,4,1,1");
  delay(20);
  simSerial.println("HAII");
  return waitForResponse("OK", 50);
}

void publishMessage(const String &topic, const String &message)
{
  int topicLength = topic.length();
  int messageLength = message.length();
  simSerial.println("AT+CMQTTTOPIC=0," + String(topicLength));
  delay(20);
  simSerial.println(topic);
  delay(20);
  simSerial.println("AT+CMQTTPAYLOAD=0," + String(messageLength));
  delay(20);
  simSerial.println(message);
  delay(20);
  simSerial.println("AT+CMQTTPUB=0,1,60"); // QoS = 1, Retain = 0
  delay(20);
}

bool waitForResponse(String expectedResponse, unsigned long timeout)
{
  unsigned long startTime = millis();
  while (millis() - startTime < timeout)
  {
    if (simSerial.available())
    {
      String response = simSerial.readString();
    //  Serial.print(response); // Imprimir respuesta para depuración
      if (response.indexOf(expectedResponse) != -1)
      {
        return true;
      }
    }
  }
  return false;
}
