#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
// retorna el valor de la mac del  bluetooth del esp32

const char* preferenceNamespace = "security"; // Namespace para las preferencias
const String defaultKey = "123456789"; // Clave por defecto

String getBluetoothMac()
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  String mac = "";
  for (int i = 0; i < 5; i++)
  {
    mac += String(baseMac[i], HEX) + ":";
  }
  mac += String(baseMac[5], HEX);
  return mac;
}

char receivedChar;// received value will be stored as CHAR in this variable
const char turnON ='ON';
const char turnOFF ='OFF';
const int LEDpin = 2;

void initBluetooth() { 
  SerialBT.begin("Lock");
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, LOW);

  preferences.begin(preferenceNamespace, false); // Abre el espacio de nombres de preferencias en modo no-read-only
  String storedKey = preferences.getString("password", defaultKey); // Recupera la clave guardada o usa la por defecto
  preferences.end(); // Cierra el espacio de nombres de preferencias
}

void loopBluetooth() {  
  if (SerialBT.available()) {
    String receivedString = SerialBT.readStringUntil('\n'); // Read the string from Bluetooth
    receivedString.trim(); // Remove any whitespace or newline characters
    if (receivedString == "AT") {
      String response = "OK"; 
      SerialBT.println(response);  
    }
    if (receivedString == "AT+MAC") {
      String response = getBluetoothMac(); 
      SerialBT.println(response); 
    } 
    if (receivedString == "AT+LOCK?") {
       if (digitalRead(LEDpin) == HIGH) {
           SerialBT.println("LOCK is OPEN");
       } else {
           SerialBT.println("LOCK is CLOSED");
       }
    }  
    if (receivedString.startsWith("AT+OPEN ")) {
           String key = receivedString.substring(10);  // Extrae la clave después de 'AT+OPEN '
          if (key == SECRET_KEY) {
                digitalWrite(LEDpin, HIGH);  // Activa el mecanismo para abrir la cerradura
                SerialBT.println("Lock opened successfully!");
          } else {
                SerialBT.println("Invalid key!");
          }
    }  
    if (receivedString.startsWith("AT+SETKEY ")) {
            String newKey = receivedString.substring(10);
            if (isValidKey(newKey)) {
                preferences.begin(preferenceNamespace);
                preferences.putString("password", newKey);
                preferences.end();
                SerialBT.println("Key updated successfully!");
            } else {
                SerialBT.println("Invalid key. Must be numeric and 9 digits long.");
            }
        }  
    
  }
}

bool isValidKey(const String& key) {
    if (key.length() != 9) return false;
    for (char c : key) {
        if (!isDigit(c)) return false;
    }
    return true;
}
