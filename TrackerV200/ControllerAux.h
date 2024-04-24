#define PIN_LED 2 // Definir el número de pin para el LED interno
#define PIN_LED_B 27
#define PIN_LED_R 26 
#define PIN_SENSOR_HALL 23 // Definir el número de pin para el sensor de efecto Hall
#define vel 200

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