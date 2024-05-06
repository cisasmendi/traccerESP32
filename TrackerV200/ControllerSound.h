#define ALARM 18     
#define BUZZER_CHANNEL 0

ToneESP32 buzzer(PIN_SONIDO, BUZZER_CHANNEL);

void setup() {
  pinMode(PIN_SONIDO, OUTPUT);  
}

 // delay(vel); // Esperar medio segundo
//  buzzer.tone(NOTE_C4, 250);