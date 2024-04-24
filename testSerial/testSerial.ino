#include <SoftwareSerial.h>
//https://tlfong01.blog/2020/03/22/sim7600-at-command-tutorial/
#define BAUD 115200
#define RXD2 16
#define TXD2 17

//SoftwareSerial Serial2 (16, 17);  // Rx, Tx as required.
#define PIN_LED 2  // Definir el número de pin para el LED interno

#define RX_GPS 14
#define TX_GPS 12
#define PIN_LED_B 27 
#define PIN_LED_R 26
#define PIN_MOTOR_ABRE 33 // Definir el número de pin D25
#define PIN_MOTOR_CIERRA 32// Definir el número de pin D25
#define MOSFET_SIM 19 // Definir el número de pin D32
#define ALARM 18 // Definir el número de pin D43
#define ON_OFF_GPS 22
#define PIN_SENSOR_HALL 23 // Definir el número de pin para el sensor de efecto Hall
#define FIN_ABIERTO 4
#define FIN_CERRADO 5
#define vel 200





void parpadear(){
    // Parpadear el LED conectado al pin 2
  digitalWrite(PIN_LED, HIGH); // Encender el LED
  delay(vel); // Esperar medio segundo
  digitalWrite(PIN_LED, LOW); // Apagar el LED
  delay(vel); // Esperar medio segundo
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
  pinMode(ON_OFF_GPS,OUTPUT);
  pinMode(PIN_SENSOR_HALL, INPUT); // Configurar el pin del sensor de efecto Hall como entrada
  pinMode(FIN_ABIERTO, INPUT);
  pinMode(FIN_CERRADO, INPUT);
 digitalWrite(PIN_LED_R, HIGH);
 digitalWrite(PIN_LED_B, HIGH);  

 
 digitalWrite(PIN_MOTOR_ABRE, LOW);
 digitalWrite(PIN_MOTOR_CIERRA, LOW);
 digitalWrite(MOSFET_SIM, HIGH);
 digitalWrite(ON_OFF_GPS, HIGH);


  Serial.begin(BAUD);
  Serial2.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
  Serial.print("Ready @ ");
  Serial.print(BAUD);
  Serial.println(" baud");
}

String completeCommand = "";

void loop()
{
  parpadear();
  if (Serial.available()){
    char received = Serial.read();
    if (received != '\n') {
      completeCommand += received;  // Agregar el carácter al comando
    } else {
      // Cuando recibimos '\n', asumimos que el comando está completo
      Serial2.println(completeCommand);  // Envía el comando completo    
      completeCommand = "";  // Limpia la cadena para el próximo comando
      }
  }

  while (Serial2.available() > 0)
    Serial.write(Serial2.read());
   
}
