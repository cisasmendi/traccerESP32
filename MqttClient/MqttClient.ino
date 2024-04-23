
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
//const char apn[] = "wap.grps.unifon.com.ar ";
//const char gprsUser[] = "wap ";
//const char gprsPass[] = "wap ";
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
  
  boolean status = mqtt.connect("GsmClientTest"); // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");
  if (status == false)
  {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  mqtt.publish(topicInit, "GsmClientTest re-started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

unsigned long previousMillis = 0;  // Variable para almacenar la última vez que el LED cambió de estado
int ledState = LOW;                // Variable para almacenar el estado actual del LED
// Función para parpadear sin bloquear
void parpadear() {
  unsigned long currentMillis = millis();
  // Comprueba si ha pasado suficiente tiempo para cambiar el estado del LED
  if (currentMillis - previousMillis >= vel) {
    // Guarda el momento del cambio de estado
    previousMillis = currentMillis;
    // Cambia el estado del LED de ON a OFF o viceversa
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // Establece el nuevo estado del LED
    digitalWrite(PIN_LED_B, ledState);
  }
}
bool reply = false;
void testModem() {
    Serial.println("\nTesting Modem Response...");
    for (int attempts = 0; attempts < 10; attempts++) {
        Serial2.println("AT");
        
        unsigned long startTime = millis();
        while (millis() - startTime < 1000) { // Espera hasta 1 segundo por una respuesta
            if (Serial2.available()) {
                String response = Serial2.readString();
                if (response.indexOf("OK") >= 0) {
                    Serial.println(response);
                    reply = true;
                    return; // Salir inmediatamente si la respuesta es correcta
                }
            }
        }
    }
    Serial.println("Modem did not respond.");
}


void testNetwork() {
    if (modem.isNetworkConnected()) {
        // Si ya está conectado a la red, verificamos GPRS directamente.
        if (!modem.isGprsConnected()) {
            Serial.println("GPRS disconnected!");
            reconnectGPRS();  // Llamamos a una función específica para reconectar GPRS.
        }
    } else {
        Serial.println("Network disconnected");
        testModem();
        
        if (modem.waitForNetwork(1000L, true)) {  // Aumentado el tiempo para un mejor manejo de la red.
            Serial.println("Network reconnected");
            if (!modem.isGprsConnected()) {
                Serial.println("GPRS disconnected!");
                reconnectGPRS();  // Reutilizamos la misma función para la reconexión de GPRS.
            }
        } else {
            Serial.println("Network reconnection failed");
        }
    }
}

void reconnectGPRS() {
    Serial.print(F("Connecting to "));
    Serial.println(apn);
    if (modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println("GPRS reconnected successfully");
    } else {
        Serial.println("GPRS reconnection failed");
    }
}

void testMQTT() {
    // Primero, verificar si ya estamos conectados a MQTT
    if (!mqtt.connected()) {
      //  Serial.println("MQTT already connected.");     
    Serial.println("=== MQTT NOT CONNECTED ===");    
    // Intentar reconectar a MQTT
    uint32_t currentTime = millis();
    if (currentTime - lastReconnectAttempt > 3000L) {
        lastReconnectAttempt = currentTime;  // Actualizar el tiempo del último intento
        if (mqttConnect()) {
            Serial.println("MQTT reconnected successfully.");
            lastReconnectAttempt = 0;  // Resetear el temporizador si la conexión fue exitosa
        } else {
            Serial.println("MQTT reconnection failed.");
            // Si la reconexión a MQTT falla, comprobamos la comunicación con el modem
            testModem();
            if (reply) {
                // Si el modem responde, verificamos y reparamos la conexión de red
                testNetwork();
                // Intentamos reconectar a MQTT una vez más después de verificar la red y el modem
                if (mqttConnect()) {
                    Serial.println("MQTT reconnected successfully after network check.");
                    lastReconnectAttempt = 0;  // Resetear el temporizador si la conexión fue exitosa
                } else {
                    Serial.println("MQTT reconnection failed after network check.");
                }
            } else {
                Serial.println("Modem not responding, cannot check network.");
            }
        }
    }}
}


void setVar(){
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
}

void setup()
{

 setVar();

  // Set console baud rate
  Serial.begin(115200);
  delay(10);
  Serial.println("Wait...");
  // Set GSM module baud rate
  // TinyGsmAutoBaud(Serial2, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  Serial2.begin(BAUD, SERIAL_8N1, RXD2, TXD2);
  delay(6000);
  Serial.println("Initializing modem...");
 
  // Variable to store the MAC address
  uint8_t baseMac[6];
  // Get the MAC address of the Bluetooth interface
  esp_read_mac(baseMac, ESP_MAC_BT);
  Serial.print("Bluetooth MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]); 

  
  modem.init();
  testModem();
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
  int hallValue = digitalRead(PIN_SENSOR_HALL); 
  if (hallValue == LOW) {
    digitalWrite(MOSFET_SIM, LOW);
    digitalWrite(ON_OFF_GPS, LOW);
    digitalWrite(PIN_LED,HIGH);
  } else {
    digitalWrite(MOSFET_SIM, HIGH);
    digitalWrite(ON_OFF_GPS, HIGH);
    testMQTT();
    mqtt.loop();
    parpadear();
    digitalWrite(PIN_LED,LOW);
  }  
  
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
