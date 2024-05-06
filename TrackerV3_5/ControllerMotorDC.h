
#define FIN_ABIERTO 4
#define FIN_CERRADO 5
#define PIN_MOTOR_ABRE 33
#define PIN_MOTOR_CIERRA 32

void initMotor()
{
    pinMode(FIN_ABIERTO, INPUT);
    pinMode(FIN_CERRADO, INPUT);
    pinMode(PIN_MOTOR_ABRE, OUTPUT);
    pinMode(PIN_MOTOR_CIERRA, OUTPUT);
    digitalWrite(PIN_MOTOR_ABRE, LOW);
    digitalWrite(PIN_MOTOR_CIERRA, LOW);
}

// fincion que abre la cerradura hata que el final de carrera abierto se active

void openLock()
{
    digitalWrite(PIN_MOTOR_ABRE, HIGH);
    while (digitalRead(FIN_ABIERTO) == 1)
    {
        delay(50);
    }
    digitalWrite(PIN_MOTOR_ABRE, LOW);
}

// fincion que cierra la cerradura hata que el final de carrera cerrado se active

void closeLock()
{
    digitalWrite(PIN_MOTOR_CIERRA, HIGH);
    while (digitalRead(FIN_CERRADO) == 1)
    {
        delay(50);
    }
    digitalWrite(PIN_MOTOR_CIERRA, LOW);
}

void toggleLock()
{
    openLock();  // Abre hasta el final de carrera abierto
    delay(1000); // Retraso opcional
    closeLock(); // Cierra hasta el final de carrera cerrado
}
