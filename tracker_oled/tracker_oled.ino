#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Crear una instancia de HardwareSerial para Serial2 sim800l
HardwareSerial mySerial(2);

// Configuración del GPS
static const int RXPin = 14, TXPin = 12;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

bool iniciarComunicacion = false;
String imei = "";

void setup() {
  // Iniciar la comunicación serie con el monitor serie
  Serial.begin(9600);

  // Configurar los pines RX y TX para Serial2
  // RX -> GPIO16, TX -> GPIO17 (puedes cambiar estos pines si es necesario)
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  // Iniciar comunicación con el GPS
  ss.begin(GPSBaud);

  delay(1000); // Esperar un segundo para asegurar que el módulo esté listo
  resetModule();
  // Obtener IMEI
  imei = obtenerIMEI();
  // Enviar comandos AT al SIM800L para conectar a Internet
  iniciarHttp();
}

void loop() {
  // Mantener el bucle vacío
  if (iniciarComunicacion) {
    while (ss.available() > 0) {
      if (gps.encode(ss.read())) {  
        if (gps.location.isValid()) {
          enviarDatos(gps.location.lat(), gps.location.lng(), imei);
        }
      }
    }

    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while (true);
    }
  }
}

void enviarDatos(double lat, double lon, String id) {
  String url = "http://104.248.226.93:5055/?lat=" + String(lat, 6) + "&lon=" + String(lon, 6) + "&id=" + id;
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\""); // Configurar URL
  sendATCommand("AT+HTTPACTION=0"); // Iniciar acción HTTP
  delay(5000); // Esperar respuesta
  sendATCommand("AT+HTTPREAD"); // Leer datos HTTP
  //  delay(10000);
  //  sendATCommand("AT+HTTPTERM"); // Terminar HTTP
}

void iniciarHttp() {
  sendATCommand("AT"); // Verificar comunicación
  sendATCommand("AT+CFUN=1"); // Fijar funcionalidad
  sendATCommand("AT+CPIN?"); // Verificar SIM
  sendATCommand("AT+CSTT=\"igprs.claro.com.ar\""); // Configurar APN
  sendATCommand("AT+CIICR"); // Iniciar la tarea de GPRS
  sendATCommand("AT+CIFSR"); // Obtener dirección IP
  sendATCommand("AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\""); // Configurar DNS
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // Configurar tipo de conexión
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"igprs.claro.com.ar\""); // Configurar APN
  sendATCommand("AT+SAPBR=1,1"); // Abrir conexión Bearer
  sendATCommand("AT+HTTPINIT"); // Inicializar HTTP
  sendATCommand("AT+HTTPPARA=\"CID\",1"); // Configurar CID

  iniciarComunicacion = true;
}

void resetModule() {
  sendATCommand("AT+CFUN=0");         // Desactivar funcionalidad del módulo
  delay(1000);
  sendATCommand("AT+CSCLK=0");        // Desactivar modo de ahorro de energía
  delay(1000);
  sendATCommand("AT&F");              // Restaurar la configuración de fábrica
  delay(1000);
  sendATCommand("AT+CFUN=1,1");       // Reiniciar el módulo
  delay(1000);
  // Esperar a que el módulo se reinicie completamente
  delay(10000);
}

String obtenerIMEI() {
  mySerial.println("AT+GSN"); // Enviar comando para obtener IMEI
  delay(1000); // Esperar respuesta

  String imei = "";
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c != '\n' && c != '\r') {
      imei += c; // Acumular caracteres del IMEI
    }
  }

  imei.trim(); // Eliminar cualquier espacio en blanco
  imei.replace("AT+GSN", ""); // Eliminar "GSN" del inicio
  imei.replace("OK", ""); // Eliminar "OK" del final
  imei.trim(); // Eliminar cualquier espacio en blanco adicional

  Serial.println("IMEI: " + imei); // Imprimir IMEI en el monitor serie
  return imei;
}

// Función para enviar comandos AT y esperar la respuesta
void sendATCommand(String command) {
  mySerial.println(command);
  Serial.print(command);
  Serial.print("->");
  delay(1000); // Esperar un segundo para la respuesta
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Imprimir la respuesta en el monitor serie
  }
}
