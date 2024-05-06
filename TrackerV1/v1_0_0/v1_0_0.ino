#include <ToneESP32.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>

// config GPS module
#define RX_GPS 14
#define TX_GPS 12

// Definir el número de pin para el LED
#define PIN_LED 2 // led interno
#define vel 200
#define PIN_LED_B 27
#define PIN_LED_R 26

// config buzzer
#define ALARM 18
#define BUZZER_CHANNEL 0
ToneESP32 buzzer(PIN_SONIDO, BUZZER_CHANNEL);

// config enabler SIM7600 and GPS
#define MOSFET_SIM 19
#define ON_OFF_GPS 22

// config sensor hall
#define PIN_SENSOR_HALL 23

// config motor
#define PIN_MOTOR_ABRE 33
#define PIN_MOTOR_CIERRA 32
#define FIN_ABIERTO 4
#define FIN_CERRADO 5

// config module SIM7600 and serial
#define BAUD 115200
#define RXD2 16
#define TXD2 17
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
// set GSM PIN, if any
#define GSM_PIN ""
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

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(Serial2, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(Serial2);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);
int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();
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
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);
  // Connect to MQTT Broker
  boolean status = mqtt.connect("GsmClientTest");
  // Or, if you want to authenticate MQTT:
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
  if (status == false)
  {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void setup()
{
  pinMode(PIN_LED, OUTPUT); // Configurar el pin del LED como salida
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_MOTOR_ABRE, OUTPUT);
  pinMode(PIN_MOTOR_CIERRA, OUTPUT);
  pinMode(MOSFET_SIM, OUTPUT);
  pinMode(PIN_SONIDO, OUTPUT);
  pinMode(ON_OFF_GPS, OUTPUT);
  pinMode(PIN_SENSOR_HALL, INPUT); // Configurar el pin del sensor de efecto Hall como entrada
  pinMode(FIN_ABIERTO, INPUT);     // Configurar el pin del switch como entrada
  pinMode(FIN_CERRADO, INPUT);     // Configurar el pin del switch como entrada

  digitalWrite(PIN_LED_R, HIGH); // Encender el LED
  digitalWrite(PIN_LED_B, HIGH); // Encender el LED
  digitalWrite(PIN_MOTOR_ABRE, LOW);
  digitalWrite(PIN_MOTOR_CIERRA, LOW);
  digitalWrite(MOSFET_SIM, HIGH);
  digitalWrite(ON_OFF_GPS, HIGH);

  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("Wait...");
  // Set GSM module baud rate
  // TinyGsmAutoBaud(Serial2, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  Serial2.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
  delay(6000);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  // modem.restart();
  modem.init();
  delay(3000);
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
#if TINY_GSM_USE_GPRS
  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    SerialMon.println("gsm_pin");
    modem.simUnlock(GSM_PIN);
  }
#endif
#if TINY_GSM_USE_GPRS
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected())
  {
    SerialMon.println("GPRS connected");
  }
#endif

  // MQTT Broker setup
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback);
}

void parpadear()
{
  // Parpadear el LED conectado al pin 2
  digitalWrite(PIN_LED, HIGH); // Encender el LED
  delay(vel);                  // Esperar medio segundo
  digitalWrite(PIN_LED, LOW);  // Apagar el LED
  delay(vel);                  // Esperar medio segundo
}


void loop()
{
  parpadear();
  loopGPRS();
}

void loopGPRS()
{
  if (!modem.isNetworkConnected())
  {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true))
    {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected())
    {
      SerialMon.println("Network re-connected");
    }

#if TINY_GSM_USE_GPRS
    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected())
    {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass))
      {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected())
      {
        SerialMon.println("GPRS reconnected");
      }
    }
#endif
  }

  if (!mqtt.connected())
  {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L)
    {
      lastReconnectAttempt = t;
      if (mqttConnect())
      {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  mqtt.loop();
}




// Leer el valor del sensor de efecto Hall
// int hallValue = digitalRead(PIN_SENSOR_HALL);
// int hallValue = digitalRead(FIN_ABIERTO);
// int hallValue = digitalRead(FIN_CERRADO);
/*   if (hallValue == HIGH) {
    digitalWrite(PIN_MOTOR_CIERRA, LOW); // Apagar el LED
      digitalWrite(PIN_MOTOR_ABRE, HIGH); // Apagar el LED
  } else {
    digitalWrite(PIN_MOTOR_CIERRA, HIGH); // Encender el LED
      digitalWrite(PIN_MOTOR_ABRE, LOW); // Apagar el LED
  }*/

//  buzzer.tone(NOTE_C4, 250);
