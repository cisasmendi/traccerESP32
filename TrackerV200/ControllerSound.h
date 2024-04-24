#define ALARM 18     
#define BUZZER_CHANNEL 0

ToneESP32 buzzer(ALARM, BUZZER_CHANNEL);

void setup() {
  pinMode(ALARM, OUTPUT);  
}

 // delay(vel); // Esperar medio segundo
//  buzzer.tone(NOTE_C4, 250);