#include "BluetoothSerial.h"
#include "ControllerPreferences.h"
#include "ControllerMotorDC.h"

BluetoothSerial SerialBT;

const int LEDpin = 2;

void initBluetooth()
{
  initMotor();
  SerialBT.begin("Lock");
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, LOW);
}

void restartDevice()
{
  esp_restart();
}

String getBluetoothMac()
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  String mac = "";
  for (int i = 0; i < 6; i++)
  {
    if (baseMac[i] < 16)
      mac += "0"; // Asegura dos dígitos hexadecimales
    mac += String(baseMac[i], HEX);
    if (i < 5)
      mac += ":";
  }
  mac.toUpperCase();
  return mac;
}

bool isValidKey(const String &key)
{
  return key.length() == 9 && key.toInt() != 0;
}


String handleATCommands(const String &command) {
  String response = "";

  if (command == "AT+?") {
    response += "Available commands:\n";
    response += "AT - Test command\n";
    response += "AT+MAC - Get the Bluetooth MAC address\n";
    response += "AT+RESET - Reset all settings and restart device\n";
    response += "AT+REBOOT - Reboot \n";
    response += "AT+OPEN [key] - Open lock with key\n";
    response += "AT+SETKEY [key] - Set a new key for lock\n";
   // response += "AT+SETWIFI [ssid],[password] - Set WiFi settings\n";
   // response += "AT+GETWIFI - Get current WiFi settings\n";
    response += "AT+SETMQTT [broker],[port],[topic] - Set MQTT settings\n";
    response += "AT+GETMQTT - Get current MQTT settings\n";
    response += "AT+SETAPN [apn],[user],[pass] - Set APN settings\n";
    response += "AT+GETAPN - Get current APN settings\n";
    response += "AT+GETCONFIG - Get all configuration settings\n";
    response += "AT+GETKEY - Get the current key lock\n";
    return response;
  }

  if (command.startsWith("AT+OPEN ")) {
    String key = command.substring(8);
    key.trim();
    String secret;
    getKeyLock(secret);
    if (key == secret) {
      digitalWrite(LEDpin, HIGH);
      response = "Lock opened successfully!";
        // open();
    } else {
      digitalWrite(LEDpin, LOW);
      response = "Invalid key!";
    }
    return response;
  }

  if (command.startsWith("AT+SETKEY ")) {
    String newKey = command.substring(10);
    newKey.trim();
    if (isValidKey(newKey)) {
      setKeyLock(newKey);
      response = "Key updated successfully!";
   
    } else {
      response = "Invalid key. Must be numeric and 9 digits long.";
    }
    return response;
  }

  if (command.startsWith("AT+SETWIFI ")) {
    int firstComma = command.indexOf(',');
    if (firstComma > 0 && firstComma < command.length() - 1) {
      String ssid = command.substring(11, firstComma);
      String pass = command.substring(firstComma + 1);
      setWiFi(ssid, pass);
      response = "WiFi settings updated successfully!";
    } else {
      response = "Invalid WiFi settings.";
    }
    return response;
  }

  if (command.startsWith("AT+GETWIFI")) {
    String ssid, pass;
    getWiFi(ssid, pass);
    response = "SSID: " + ssid + ", PASS: " + pass;
    return response;
  }

  if (command.startsWith("AT+SETMQTT ")) {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0) {
      String broker = command.substring(11, firstComma);
      int port = command.substring(firstComma + 1, secondComma).toInt();
      String topic = command.substring(secondComma + 1);
      setConfigMqtt(broker, port, topic);
      response = "MQTT settings updated successfully!";
    } else {
      response = "Invalid MQTT settings.";
    }
    return response;
  }

  if (command.startsWith("AT+GETMQTT")) {
    String broker;
    int port;
    String topic;
    getConfigMqtt(broker, port, topic);
    response = "MQTT Broker: " + broker + ", Port: " + String(port) + ", Topic: " + topic;
    return response;
  }

  if (command.startsWith("AT+SETAPN ")) {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0) {
      String apn = command.substring(10, firstComma);
      String gprsUser = command.substring(firstComma + 1, secondComma);
      String gprsPass = command.substring(secondComma + 1);
      setConfigApn(apn, gprsUser, gprsPass);
      response = "APN settings updated successfully!";
    } else {
      response = "Invalid APN settings.";
    }
    return response;
  }

  if (command.startsWith("AT+GETAPN")) {
    String apn, gprsUser, gprsPass;
    getConfigApn(apn, gprsUser, gprsPass);
    response = "APN: " + apn + ", USER: " + gprsUser + ", PASS: " + gprsPass;
    return response;
  }

  if (command == "AT+GETCONFIG") {
    // The method getConfig needs to be created according to your system's specifications
    response = getConfig(); // Assumed a hypothetical getConfig() method returning a formatted string of all settings
    return response;
  }

  if (command == "AT+GETKEY") {
    String secret;
    getKeyLock(secret);
    response = "Key Lock: " + secret;
    return response;
  }

  return "Unknown command"; // Default case if no command matches
}


String handleCommands(const String& command) {
    String response;  // Variable para almacenar la respuesta

    if (command == "AT") {
        response = "OK";
    }
    else if (command == "AT+MAC") {
        response = getBluetoothMac();  // Supone una función que obtiene la MAC del Bluetooth
    }
    else if (command == "AT+RESET") {
        resetConfig();  // Supone una función que reinicia la configuración
        response = "All settings have been reset.";
        delay(5000);
        restartDevice();  // Supone una función que reinicia el dispositivo
    }else if (command == "AT+REBOOT"){
        restartDevice();
    }
    else if (command.startsWith("AT+")) {
        response = handleATCommands(command);  // Supone una función que maneja otros comandos AT+
    }
    else {
        response = "Unknown command";
    }

    return response;
}


void loopBluetooth()
{
  if (SerialBT.available()){
    String receivedString = SerialBT.readStringUntil('\n');
    receivedString.trim();  // Limpia espacios en blanco
    String response = handleCommands(receivedString);  // Procesa el comando y obtiene la respuesta
    SerialBT.println(response);  // Envía la respuesta
  }
}
