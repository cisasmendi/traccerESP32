#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoHttpClient.h>

// config enabler SIM7600 and GPS
#define MOSFET_SIM 19
#define ON_OFF_GPS 22

// Configura aquí tus credenciales de la red GSM
const char apn[]      = "igprs.claro.com.ar";
const char user[]     = "";
const char pass[]     = "";

// Configura aquí los detalles del servidor MQTT
const char* broker = "test.mosquitto.org";
const int port = 1883;

// Pin RX y TX para el módem
#define MODEM_RX_PIN 16
#define MODEM_TX_PIN 17



// Servidor web y ruta para la solicitud HTTP
const char server[] = "v2.jokeapi.dev";
const char resource[] = "/joke/Any";
const int port1 = 80;
// Número de teléfono y mensaje para el SMS
const char phoneNumber[] = "+54383154358706"; // Cambia al número de destino real
const char message[] = "Hello from esp32";

// Define el cliente GSM y MQTT
TinyGsm modem(Serial2);
TinyGsmClient client(modem);
PubSubClient mqtt;
HttpClient http(client, server, port1);

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Mensaje recibido en el tema: ");
  Serial.println(topic);
  Serial.print("Mensaje:");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  pinMode(MOSFET_SIM, OUTPUT);
  pinMode(ON_OFF_GPS, OUTPUT);
  digitalWrite(MOSFET_SIM, HIGH);
  digitalWrite(ON_OFF_GPS, HIGH);
  delay(3000);
  // Inicia el Serial
  Serial.begin(115200);
  delay(1000);
  // Inicia la comunicación con el módem
  
  Serial2.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  // Reinicia y inicializa el módem
  Serial.println("Inicializando módem...");
  modem.init(); 
  delay(3000);
  //Serial.print("reset: ");
  //Serial.println(modem.factoryDefault());
  mqtt.setClient(client);
  delay(3000);
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
 
  // Conexión GPRS
  Serial.print("Conectando a la red GPRS...");
  if (!modem.waitForNetwork())
  {
    Serial.println("falla");
    delay(10000);
    return;
  }
  Serial.println(" conectado"); 
 // Enviar un SMS
  sendSMS(phoneNumber, message);
//  mqtt.setServer(broker, port);
//  mqtt.setCallback(mqttCallback);
  
/*

 // Llama a la función que realiza la solicitud HTTP GET
  testHttpGet();
   
  // Conexión MQTT
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback);
  while(!mqtt.connect("GsmClientTest")) {
    Serial.println("Conexión MQTT fallida");   
  }

  Serial.println("Conectado a MQTT");
  mqtt.subscribe("GsmClientTest");*/
}

void loop() {
/*  if (!mqtt.connected()) {
    Serial.println("Desconectado de MQTT. Reintentando...");
    while (!mqtt.connect("GsmClientTest")) {
      Serial.print("Conectando a la red GPRS...");
      if (!modem.waitForNetwork())
      {
       Serial.println("falla");
       delay(10000);
       return;
       }
      Serial.println(" conectado");    
       checkMQTTConnectionState();    
      // Consulta del estado del módem
      // checkModemStatus();
      delay(5000);
    }
    mqtt.subscribe("GsmClientTest");
  }

  mqtt.loop();*/
}

void testHttpGet() {
  Serial.println("Performing HTTP GET request...");
  int httpCode = http.get(resource);
  if (httpCode > 0) {
    // HTTP header has been sent and Server response header has been handled
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    String payload = http.responseBody();
    Serial.println("HTTP Response body: ");
    Serial.println(payload);
  } else {
    Serial.print("HTTP GET request failed: ");
    Serial.println(httpCode);
  }
}

void sendSMS(const char* num, const char* msg) {
  Serial.println("Sending SMS...");

  if (modem.sendSMS(num, msg)) {
    Serial.println("SMS sent successfully!");
  } else {
    Serial.println("SMS failed to send");
  }
}

void checkModemStatus() {
  // info modem
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  
  // Consultar la calidad de la señal
  int signalQuality = modem.getSignalQuality();
  Serial.print("Signal quality: ");
  Serial.println(signalQuality);


  // Consultar el nombre del operador
  String operatorName = modem.getOperator();
  Serial.print("Operator: ");
  Serial.println(operatorName);

  // Consultar el IMEI del módem
  String imei = modem.getIMEI();
  Serial.print("IMEI: ");
  Serial.println(imei);

  // Consultar el CCID de la tarjeta SIM
  String ccid = modem.getSimCCID();
  Serial.print("SIM CCID: ");
  Serial.println(ccid);

  // Consultar y mostrar la dirección IP
  String ip = modem.getLocalIP();
  Serial.print("Local IP address: ");
  Serial.println(ip);
}

void checkMQTTConnectionState() {
    int state = mqtt.state();
    Serial.print("MQTT Connection State: ");
    switch(state) {
        case -4:
            Serial.println("MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time");
            break;
        case -3:
            Serial.println("MQTT_CONNECTION_LOST - the network connection was broken");
            break;
        case -2:
            Serial.println("MQTT_CONNECT_FAILED - the network connection failed");
            break;
        case -1:
            Serial.println("MQTT_DISCONNECTED - the client is disconnected cleanly");
            break;
        case 0:
            Serial.println("MQTT_CONNECTED - the client is connected");
            break;
        case 1:
            Serial.println("MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT");
            break;
        case 2:
            Serial.println("MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier");
            break;
        case 3:
            Serial.println("MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection");
            break;
        case 4:
            Serial.println("MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected");
            break;
        case 5:
            Serial.println("MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect");
            break;
        default:
            Serial.println("Unknown state.");
            break;
    }
}
