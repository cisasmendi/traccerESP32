#include "BluetoothSerial.h"
#include "ControllerPreferences.h"

BluetoothSerial SerialBT;

const int LEDpin = 2;

void initBluetooth()
{
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



void handleATCommands(const String &command)
{
  if (command == "AT+?")
  {
    SerialBT.println(F("Available commands:"));
    SerialBT.println(F("AT - Test command"));
    SerialBT.println(F("AT+MAC - Get the Bluetooth MAC address"));
    SerialBT.println(F("AT+RESET - Reset all settings and restart device"));
    SerialBT.println(F("AT+OPEN [key] - Open lock with key"));
    SerialBT.println(F("AT+SETKEY [key] - Set a new key for lock"));
    SerialBT.println(F("AT+SETWIFI [ssid],[password] - Set WiFi settings"));
    SerialBT.println(F("AT+GETWIFI - Get current WiFi settings"));
    SerialBT.println(F("AT+SETMQTT [broker],[port],[topic] - Set MQTT settings"));
    SerialBT.println(F("AT+GETMQTT - Get current MQTT settings"));
    SerialBT.println(F("AT+SETAPN [apn],[user],[pass] - Set APN settings"));
    SerialBT.println(F("AT+GETAPN - Get current APN settings"));
    SerialBT.println(F("AT+GETCONFIG - Get all configuration settings"));
    SerialBT.println(F("AT+GETKEY - Get the current key lock"));

    return;
  }
  if (command.startsWith("AT+OPEN "))
  {
    String key = command.substring(10); // Extrae la parte del comando que contiene la clave
    key.trim();                         // Limpia espacios en blanco al principio y al final de la clave
    String secret;
    getKeyLock(secret);
    if (key == secret)
    {
      digitalWrite(LEDpin, HIGH);
      SerialBT.println(F("Lock opened successfully!"));
    }
    else
    {
       digitalWrite(LEDpin, LOW);
      SerialBT.println(F("Invalid key!"));
    }
    return;
  }
  if (command.startsWith("AT+SETKEY "))
  {
    String newKey = command.substring(10); // Extrae la nueva clave
    newKey.trim();                         // Limpia espacios en blanco
    if (isValidKey(newKey))
    {
      setKeyLock(newKey);
      SerialBT.println(F("Key updated successfully!"));
    }
    else
    {
      SerialBT.println(F("Invalid key. Must be numeric and 9 digits long."));
    }
    return;
  }
  if (command.startsWith("AT+SETWIFI "))
  {
    int firstComma = command.indexOf(',');
    //  int secondComma = command.indexOf(',', firstComma + 1);

    if (firstComma > 0 && firstComma < command.length() - 1)
    {
      String ssid = command.substring(11, firstComma);
      String pass = command.substring(firstComma + 1);
      setWiFi(ssid, pass);
      SerialBT.println(F("WiFi settings updated successfully!"));
    }
    else
    {
      SerialBT.println(F("Invalid WiFi settings."));
    }
    return;
  }

  if (command.startsWith("AT+GETWIFI"))
  {
    String ssid;
    String pass;
    getWiFi(ssid, pass);
    SerialBT.println("SSID: " + ssid + ", PASS: " + pass);
    return;
  }

  if (command.startsWith("AT+SETMQTT "))
  {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0)
    {
      String broker = command.substring(11, firstComma);
      int port = command.substring(firstComma + 1, secondComma).toInt();
      String topic = command.substring(secondComma + 1);
      setConfigMqtt(broker, port, topic);
      SerialBT.println(F("MQTT settings updated successfully!"));
    }
    else
    {
      SerialBT.println(F("Invalid MQTT settings."));
    }
    return;
  }
  if (command.startsWith("AT+GETMQTT"))
  {
    String broker;
    int port;
    String topic;
    getConfigMqtt(broker, port, topic);
    SerialBT.println("MQTT Broker: " + broker + ", Port: " + String(port) + ", Topic: " + topic);
    return;
  }

  if (command.startsWith("AT+SETAPN "))
  {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0)
    {
      String apn = command.substring(10, firstComma);
      String gprsUser = command.substring(firstComma + 1, secondComma);
      String gprsPass = command.substring(secondComma + 1);
      setConfigApn(apn, gprsUser, gprsPass);
      SerialBT.println(F("APN settings updated successfully!"));
    }
    else
    {
      SerialBT.println(F("Invalid APN settings."));
    }
    return;
  }
  if (command.startsWith("AT+GETAPN"))
  {
    String apn, gprsUser, gprsPass;
    getConfigApn(apn, gprsUser, gprsPass);
    SerialBT.println("APN: " + apn + ", USER: " + gprsUser + ", PASS: " + gprsPass);
    return;
  }

  if (command == "AT+GETCONFIG")
  {
    bool wifienable;
    String ssid, password, secret, broker, topic, apn, gprsUser, gprsPass;
    int port;
    bool reset;
    String config = getConfig(wifienable, ssid, password, secret, broker, port, topic, apn, gprsUser, gprsPass, reset);
    SerialBT.println(config);
    return;
  }
  if (command == "AT+GETKEY")
  {
    String secret;
    getKeyLock(secret);
    SerialBT.println("Key Lock: " + secret);
    return;
  }
}

void loopBluetooth()
{
  if (SerialBT.available())
  {
    String receivedString = SerialBT.readStringUntil('\n');
    receivedString.trim();

    if (receivedString == "AT")
    {
      SerialBT.println(F("OK"));
    }
    else if (receivedString == "AT+MAC")
    {
      SerialBT.println(getBluetoothMac());
    }
    else if (receivedString == "AT+RESET")
    {
      resetConfig();
      SerialBT.println(F("All settings have been reset."));
      delay(5000);
      restartDevice();
    }
    else if (receivedString.startsWith("AT+"))
    {
      handleATCommands(receivedString);
    }
    else
    {
      SerialBT.println(F("Unknown command"));
    }
  }
}
