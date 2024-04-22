
#define TINY_GSM_MODEM_SIM7600

#define BAUD 115200
#define RXD2 16
#define TXD2 17

#define PIN_LED 2 // Definir el número de pin para el LED interno

#define RX_GPS 14
#define TX_GPS 12
#define PIN_LED_B 27
#define PIN_LED_R 26
#define PIN_MOTOR_ABRE 33   // Definir el número de pin D25
#define PIN_MOTOR_CIERRA 32 // Definir el número de pin D25
#define MOSFET_SIM 19       // Definir el número de pin D32
#define ALARM 18            // Definir el número de pin D43
#define ON_OFF_GPS 22
#define PIN_SENSOR_HALL 23 // Definir el número de pin para el sensor de efecto Hall
#define FIN_ABIERTO 4
#define FIN_CERRADO 5
#define vel 200

//#define TINY_GSM_DEBUG Serial

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200
// Your GPRS credentials, if any
const char apn[] = "igprs.claro.com.ar";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char *broker = "64.226.117.238";
const int port = 1883;
const char *topicLed = "GsmClientTest/led";
const char *topicInit = "GsmClientTest/init";
const char *topicLedStatus = "GsmClientTest/ledStatus";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

TinyGsm modem(Serial2);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

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

boolean mqttConnect()
{
  Serial.print("Connecting to ");
  Serial.print(broker);
  // Connect to MQTT Broker
  boolean status = mqtt.connect("GsmClientTest");
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
  if (status == false)
  {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void parpadear()
{
  // Parpadear el LED conectado al pin 2
  digitalWrite(PIN_LED, HIGH); // Encender el LED
  delay(vel);                  // Esperar medio segundo
  digitalWrite(PIN_LED, LOW);  // Apagar el LED
  delay(vel);                  // Esperar medio segundo
}

bool reply = false;

void testConect()
{
  int i = 10;
  Serial.println("\nTesting Modem Response...");
  while (i)
  {
    Serial2.println("AT");
    delay(500);
    if (Serial2.available())
    {
      String r = Serial2.readString();
      //Serial.println(r);
      if (r.indexOf("OK") >= 0)
      {
        Serial.println(r);
        reply = true;
        break;
        ;
      }
    }
    delay(500);
    i--;
  }
}

void testNetwork()
{
  if (!modem.isNetworkConnected())
  {
    Serial.println("Network disconnected");
    testConect();
    if (!modem.waitForNetwork(1000L, true))
    {
      Serial.println(" fail");
      return;
    } 
    if (!modem.isGprsConnected())
    {
      Serial.println("GPRS disconnected!");
      Serial.print(F("Connecting to "));
      Serial.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass))
      {
        Serial.println(" fail");    
        return;
      }
      if (modem.isGprsConnected())
      {
        Serial.println("GPRS reconnected");
      }
    }
  }
}

void testMQTT()
{
  if (!mqtt.connected())
  {
    Serial.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 3000L)
    {
      lastReconnectAttempt = t;
      if (mqttConnect())
      {
        lastReconnectAttempt = 0;
      }
    }  
    return;
  }
}

void setup()
{

  pinMode(PIN_LED, OUTPUT); // Configurar el pin del LED como salida
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_MOTOR_ABRE, OUTPUT);
  pinMode(PIN_MOTOR_CIERRA, OUTPUT);
  pinMode(MOSFET_SIM, OUTPUT);
  pinMode(ALARM, OUTPUT);
  pinMode(ON_OFF_GPS, OUTPUT);
  pinMode(PIN_SENSOR_HALL, INPUT); // Configurar el pin del sensor de efecto Hall como entrada
  pinMode(FIN_ABIERTO, INPUT);
  pinMode(FIN_CERRADO, INPUT);
  digitalWrite(PIN_LED_R, HIGH);
  digitalWrite(PIN_LED_B, HIGH);

  digitalWrite(PIN_MOTOR_ABRE, LOW);
  digitalWrite(PIN_MOTOR_CIERRA, LOW);
  digitalWrite(MOSFET_SIM, HIGH);
  digitalWrite(ON_OFF_GPS, HIGH);

  // Set console baud rate
  Serial.begin(115200);
  delay(10);
  Serial.println("Wait...");
  // Set GSM module baud rate
  // TinyGsmAutoBaud(Serial2, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  Serial2.begin(BAUD, SERIAL_8N1, RXD2, TXD2);
  delay(6000);
  Serial.println("Initializing modem...");
  modem.init();
  testConect();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);
  modem.gprsConnect(apn, gprsUser, gprsPass);
  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" success");

  if (modem.isNetworkConnected())
  {
    Serial.println("Network connected");
  }
  // MQTT Broker setup
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback);
}

void loop()
{
  parpadear();
  testNetwork();
  testMQTT();
  mqtt.loop();
}
