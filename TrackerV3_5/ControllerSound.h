#include <ToneESP32.h>
#define PIN_SONIDO 18
#define BUZZER_CHANNEL 0

ToneESP32 buzzer(PIN_SONIDO, BUZZER_CHANNEL);


void melodyOK()
{
  buzzer.tone(NOTE_C4, 100); 
  buzzer.tone(NOTE_E5, 200);
}

void melodyError()
{
  buzzer.tone(NOTE_C3, 300);
  delay(50);
  buzzer.tone(NOTE_C3, 300); 
}

void melodyConnect()
{
  buzzer.tone(NOTE_C4, 200);
  delay(50);
  buzzer.tone(NOTE_E4, 200);
  delay(50);
  buzzer.tone(NOTE_G4, 200);
}

void testSound()
{
  melodyOK();
  delay(3000);
  melodyError();
  delay(3000);
  melodyConnect();
}

void alarm()
{ 
  buzzer.tone(NOTE_C3, 300);
  buzzer.tone(NOTE_C4, 100);  
  buzzer.tone(NOTE_C3, 300);
  buzzer.tone(NOTE_C4, 100);
}

void initSound()
{
  pinMode(PIN_SONIDO, OUTPUT);
  melodyConnect();
}
