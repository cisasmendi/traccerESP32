#include "ControllerPreferences.h"

const int LEDpin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, LOW);
}

String getBluetoothMac() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  String mac = "";
  for (int i = 0; i < 6; i++) {
    if (baseMac[i] < 16) mac += "0";  // Asegura dos dígitos hexadecimales
    mac += String(baseMac[i], HEX);
    if (i < 5) mac += ":";
  }
  mac.toUpperCase();
  return mac;
}

bool isValidKey(const String& key) {
    return key.length() == 9 && key.toInt() != 0;
}

void restartDevice() {   
    esp_restart();
}

void loop() {
  if (Serial.available()) {
    String receivedString = Serial.readStringUntil('\n');
    receivedString.trim();
  
    if (receivedString == "AT") {
      Serial.println(F("OK"));
    } else if (receivedString == "AT+MAC") {
      Serial.println(getBluetoothMac());
    } else if (receivedString == "AT+RESET") {
      resetConfig();
      Serial.println(F("All settings have been reset."));
      delay(5000);
      restartDevice();       
    } else if (receivedString.startsWith("AT+")) {
      handleATCommands(receivedString);
    } else {
      Serial.println(F("Unknown command"));
    }
  }
}

void handleATCommands(const String& command) {
    if (command == "AT+?") {
            Serial.println(F("Available commands:"));
    Serial.println(F("AT - Test command"));
    Serial.println(F("AT+MAC - Get the Bluetooth MAC address"));
    Serial.println(F("AT+RESET - Reset all settings and restart device"));
    Serial.println(F("AT+OPEN [key] - Open lock with key"));
    Serial.println(F("AT+SETKEY [key] - Set a new key for lock"));
    Serial.println(F("AT+SETWIFI [ssid],[password] - Set WiFi settings"));
    Serial.println(F("AT+GETWIFI - Get current WiFi settings"));
    Serial.println(F("AT+SETMQTT [broker],[port],[topic] - Set MQTT settings"));
    Serial.println(F("AT+GETMQTT - Get current MQTT settings"));
    Serial.println(F("AT+SETAPN [apn],[user],[pass] - Set APN settings"));
    Serial.println(F("AT+GETAPN - Get current APN settings"));
    Serial.println(F("AT+GETCONFIG - Get all configuration settings"));
    Serial.println(F("AT+GETKEY - Get the current key lock"));

        return;
    }  
  if (command.startsWith("AT+OPEN ")) {
    String key = command.substring(10); // Extrae la parte del comando que contiene la clave
    key.trim(); // Limpia espacios en blanco al principio y al final de la clave
    String secret;
    getKeyLock(secret);
    if (key == secret) {
      digitalWrite(LEDpin, HIGH);
      Serial.println(F("Lock opened successfully!"));
    } else {
      Serial.println(F("Invalid key!"));
    }
    return;
  }
  if (command.startsWith("AT+SETKEY ")) {
    String newKey = command.substring(10); // Extrae la nueva clave
    newKey.trim(); // Limpia espacios en blanco
    if (isValidKey(newKey)) {
      setKeyLock(newKey);
      Serial.println(F("Key updated successfully!"));
    } else {
      Serial.println(F("Invalid key. Must be numeric and 9 digits long."));
    }
    return;
  }
   if (command.startsWith("AT+SETWIFI ")) {
      int firstComma = command.indexOf(',');
    //  int secondComma = command.indexOf(',', firstComma + 1);
    
      if (firstComma > 0 && firstComma < command.length() - 1) {
        String ssid = command.substring(11, firstComma);
        String pass = command.substring(firstComma + 1);
        setWiFi(ssid, pass);
        Serial.println(F("WiFi settings updated successfully!"));
      } else {
        Serial.println(F("Invalid WiFi settings."));
      }
      return;
    }

    if (command.startsWith("AT+GETWIFI")) {
      String ssid;
      String pass;
      getWiFi(ssid, pass);
      Serial.println("SSID: " + ssid + ", PASS: " + pass);
      return;
    }

    if (command.startsWith("AT+SETMQTT ")) {
        int firstComma = command.indexOf(',');
        int secondComma = command.indexOf(',', firstComma + 1);
        if (firstComma > 0 && secondComma > 0) {
            String broker = command.substring(11, firstComma);
            int port = command.substring(firstComma + 1, secondComma).toInt();
            String topic = command.substring(secondComma + 1);
            setConfigMqtt(broker, port, topic);
            Serial.println(F("MQTT settings updated successfully!"));
        } else {
            Serial.println(F("Invalid MQTT settings."));
        }
        return;
    } 
    if (command.startsWith("AT+GETMQTT")) {
        String broker;
        int port;
        String topic;
        getConfigMqtt(broker, port, topic);
        Serial.println("MQTT Broker: " + broker + ", Port: " + String(port) + ", Topic: " + topic);
        return;
    }
    
  if (command.startsWith("AT+SETAPN ")) {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0) {
        String apn = command.substring(10, firstComma);
        String gprsUser = command.substring(firstComma + 1, secondComma);
        String gprsPass = command.substring(secondComma + 1);
        setConfigApn(apn, gprsUser, gprsPass);
        Serial.println(F("APN settings updated successfully!"));
    } else {
        Serial.println(F("Invalid APN settings."));
    }
    return;
}
if (command.startsWith("AT+GETAPN")) {
    String apn, gprsUser, gprsPass;
    getConfigApn(apn, gprsUser, gprsPass);
    Serial.println("APN: " + apn + ", USER: " + gprsUser + ", PASS: " + gprsPass);
    return;
}

if (command == "AT+GETCONFIG") {
    bool wifienable;
    String ssid, password, secret, broker, topic, apn, gprsUser, gprsPass;
    int port;
    bool reset;
    String config = getConfig(wifienable, ssid, password, secret, broker, port, topic, apn, gprsUser, gprsPass, reset);
    Serial.println(config);
    return;
}
if (command == "AT+GETKEY") {
    String secret;
    getKeyLock(secret);
    Serial.println("Key Lock: " + secret);
    return;
}

}
