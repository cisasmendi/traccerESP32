#define TINY_GSM_MODEM_SIM7600
#define MOSFET_SIM 19
#define BAUD 115200
#define RXD2_SIMX 16
#define TXD2_SIMX 17

// Your GPRS credentials, if any
char apn[] = "";
char gprsUser[] = "";
char gprsPass[] = "";

// GSM details
#include <TinyGsmClient.h>
TinyGsm modem(Serial2);
TinyGsmClient client(modem);

boolean connectGPRS();
boolean testModem();
boolean testNetwork();

// verifica la comunicacion con el modem retorna true si la comunicacion es exitosa usando
boolean testModem()
{
    // Serial.println("\nTesting Modem Response...");
    unsigned long startTime = millis();
    do
    {
        if (!modem.testAT())
        {
            delay(1000);
            // Serial.println("Modem did not respond.");
            return false;
        }
        else
        {
            // Serial.println("Modem responded.");
            return true;
        }

    } while (millis() - startTime < 1000);
}
// verifica la conexion con la red y si no esta conectado intenta reconectar
boolean testNetwork()
{
    if (modem.isNetworkConnected())
    {
        if (!modem.isGprsConnected())
        {
            // Serial.println(F("GPRS disconnected!"));
            return connectGPRS();
        }
    }
    else
    {
        // Serial.println(F("Network disconnected"));
        if (modem.waitForNetwork(1000L, true))
        {
            // Serial.println(F("Network reconnected"));
            if (!modem.isGprsConnected())
            {
                // Serial.println(F("GPRS disconnected!"));
                return connectGPRS();
            }
        }
        else
        {
            // Serial.println(F("Network reconnection failed"));
            return false;
        }
    }
    return true;
}
// reconecta el GPRS
boolean connectGPRS()
{
    // Serial.print(F("Connecting to "));
    // Serial.println(apn);
    if (modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        // Serial.println(F("GPRS reconnected successfully"));
        return true;
    }
    else
    {
        // Serial.println(F("GPRS reconnection failed"));
        return false;
    }
}


void initSim(String apn, String gprsUser, String gprsPass)
{

    char apnArray[apn.length() + 1];
    char gprsUserArray[gprsUser.length() + 1];
    char gprsPassArray[gprsPass.length() + 1];
    // Copia las cadenas a los arrays
    apn.toCharArray(apnArray, sizeof(apnArray));
    gprsUser.toCharArray(gprsUserArray, sizeof(gprsUserArray));
    gprsPass.toCharArray(gprsPassArray, sizeof(gprsPassArray));
    
    pinMode(MOSFET_SIM, OUTPUT);
    digitalWrite(MOSFET_SIM, LOW);
    Serial2.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
    delay(5000);
    modem.init();
    testModem(); 
}
