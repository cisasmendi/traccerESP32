#include <ToneESP32.h>
#define PIN_SONIDO 18
#define BUZZER_CHANNEL 0

ToneESP32 buzzer(PIN_SONIDO, BUZZER_CHANNEL);

void beep(int note, int duree)
{
  buzzer.tone(note, duree);
  buzzer.noTone();
  delay(duree * 0.25);
}

void melodyOK()
{
  beep(NOTE_G4, 100);
  beep(NOTE_E5, 200);
}

void melodyError()
{
  beep(NOTE_C3, 300);
  delay(50);
  beep(NOTE_C3, 300);
}

void melodyConnect()
{
  beep(NOTE_C4, 200);
  delay(50);
  beep(NOTE_E4, 200);
  delay(50);
  beep(NOTE_G4, 200);
}

void testSound()
{
  melodyOK();
  melodyError();
  melodyConnect();
}

void initSound()
{
  pinMode(PIN_SONIDO, OUTPUT);
  testSound();
}


