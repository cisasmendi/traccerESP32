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
boolean testModem(){
   //Serial.println("\nTesting Modem Response...");
    unsigned long startTime = millis();
    do
    {
        if (!modem.testAT()){
            delay(1000);
            //Serial.println("Modem did not respond.");
            return false;
        }else{
            //Serial.println("Modem responded.");
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
            //Serial.println(F("GPRS disconnected!"));
            return connectGPRS();
        }
    }
    else
    {
        //Serial.println(F("Network disconnected"));
        if (modem.waitForNetwork(1000L, true))
        {
            //Serial.println(F("Network reconnected"));
            if (!modem.isGprsConnected())
            {
                //Serial.println(F("GPRS disconnected!"));
                return connectGPRS();
            }
        }
        else
        {
            //Serial.println(F("Network reconnection failed"));
            return false;
        }
    }
    return true;
}
// reconecta el GPRS
boolean connectGPRS( )
{
    //Serial.print(F("Connecting to "));
    //Serial.println(apn);
    if (modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        //Serial.println(F("GPRS reconnected successfully"));
        return true;
    }
    else
    {
        //Serial.println(F("GPRS reconnection failed"));
        return false;
    }
}


void checkModemStatus() {
  // info modem
  String modemInfo = modem.getModemInfo();
  //Serial.print(F("Modem: "));
  //Serial.println(modemInfo);
  
  // Consultar la calidad de la señal
  int signalQuality = modem.getSignalQuality();
  //Serial.print(F("Signal quality: "));
  //Serial.println(signalQuality);


  // Consultar el nombre del operador
  String operatorName = modem.getOperator();
  //Serial.print(F("Operator: "));
  //Serial.println(operatorName);

  // Consultar el CCID de la tarjeta SIM
  String ccid = modem.getSimCCID();
  //Serial.print(F("SIM CCID: "));
  //Serial.println(ccid);

  // Consultar y mostrar la dirección IP
  String ip = modem.getLocalIP();
  //Serial.print(F("Local IP address: "));
  //Serial.println(ip);

  float vBatt = modem.getBattVoltage() / 1000.0;
  //Serial.print(F("Bateri: "));
  //Serial.println(vBatt); 
  
  // Consultar el IMEI del módem
  String imei = modem.getIMEI();
  //Serial.print(F("IMEI: "));
  //Serial.println(imei);
}

void initSim(String apn, String gprsUser, String gprsPass)
{

    apn.toCharArray(apn, apn.length() + 1);
    gprsUser.toCharArray(gprsUser, gprsUser.length() + 1);
    gprsPass.toCharArray(gprsPass, gprsPass.length() + 1);

    pinMode(MOSFET_SIM, OUTPUT);
    digitalWrite(MOSFET_SIM, HIGH);   
    Serial2.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);
    delay(5000);
    modem.init();  
    if (testModem()){
       // checkModemStatus();
        delay(1000);       
    }
    
}
