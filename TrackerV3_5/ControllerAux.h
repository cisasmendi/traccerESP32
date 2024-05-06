#define PIN_LED 2 // Definir el número de pin para el LED interno
#define PIN_LED_B 27
#define PIN_LED_R 26
#define PIN_SENSOR_HALL 23 // Definir el número de pin para el sensor de efecto Hall
#define vel 200

unsigned long previousMillis = 0; // Variable para almacenar la última vez que el LED cambió de estado
// Función para parpadear sin bloquear
bool isBlinking = false; 

// Función para parpadear sin bloquear
bool state = 1;
void parpadear() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= vel) {
        previousMillis = currentMillis;
        state = !state;
        digitalWrite(PIN_LED_B, state);  // Cambiar el estado del LED
    }
}

void loopAux(){
  if(isBlinking)
  parpadear();
}

// Función para comenzar el parpadeo del LED
void startBlink() {
    isBlinking = true;
}

// Función para detener el parpadeo del LED
void stopBlink() {
    isBlinking = false;
    digitalWrite(PIN_LED_B, HIGH);  // Asegurarse de que el LED queda apagado al detener el parpadeo
}

void initAux()
{
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_SENSOR_HALL, INPUT);
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_R, HIGH);
}
