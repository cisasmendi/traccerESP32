#define MOSFET_SIM 19
#define BAUD 115200
#define RXD2_SIMX 16
#define TXD2_SIMX 17


// Your GPRS credentials, if any
/*char apn[] = "";
char gprsUser[] = "";
char gprsPass[] = "";*/

bool waitForResponse(const String& expectedResponse, unsigned long timeout, const bool debug) {
    unsigned long startTime = millis();
    String response;
    while (millis() - startTime < timeout) {
        while (Serial2.available()) {
            char c = Serial2.read();
            response += c;
            if (response.indexOf(expectedResponse) >= 0) {
               if(debug)
                Serial.println(response);  // Imprimir la respuesta para depuración
                return true;
            }
        }
    }
  //  Serial.println(response);  // Imprimir la respuesta si se alcanza el tiempo de espera
    return false;  // Retorna falso si se agota el tiempo y no se encuentra la respuesta esperada
}

String waitResponse(const String& expectedResponse, unsigned long timeout, const bool debug) {
    unsigned long startTime = millis();
    String response="";
    while (millis() - startTime < timeout) {
        while (Serial2.available()) {
            char c = Serial2.read();
            response += c;
            if (response.indexOf(expectedResponse) >= 0) {
               if(debug)
                Serial.println(response);  // Imprimir la respuesta para depuración
                //return response;
            }
        }
    }
    return response;  
}


bool sendATCommand(const String& command, const String& expectedResponse, int attempts = 10, unsigned long timeout = 500, const bool debug = true, const bool typeString = true ) {
    if(!typeString){
    while (attempts-- > 0) {
        Serial2.println(command);
        if (waitForResponse(expectedResponse, timeout, debug)) {
            return true;
        }
    }
    return false;
    }else{
      while (attempts-- > 0) {
        Serial2.println(command);
        return waitResponse(expectedResponse, timeout, debug);
    }
    }
}

bool setupAPN(const String &apn, const String &username, const String &password)
{   
  if (username.length() > 0 && password.length() > 0){      
    sendATCommand("AT+CGAUTH=1,1,\"" + username + "\",\"" + password + "\"", "OK", 1, 50);      
  }else{
     if (sendATCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\"", "OK", 1, 50)) {  
        } 
     if(sendATCommand("AT+CGACT=1,1","OK", 10, 1000)){
        return true;
     }   
  }     
  return false;
}


bool connectToServer(const String &clientId, const String &server, int port)
{
 // Serial2.println("ATE0");
  delay(20);
  Serial2.println("AT+CMQTTSTART");
  delay(20);
  Serial2.println("AT+CMQTTACCQ=0,\"" + clientId + "\"");
  delay(20);
  Serial2.println("AT+CMQTTCONNECT=0,\"tcp://" + server + ":" + String(port) + "\",90,1");  
  return true;
}
String getIMEI(){ 
 sendATCommand("AT+SIMEI?","OK",1, 50 , true);
}

bool subscribeToTopic(const String &topic)
{
  int topicLength = topic.length();
  Serial2.println("AT+CMQTTSUBTOPIC=0," + String(topicLength) + ",1");
  delay(20);
  Serial2.println(topic);
  delay(20);
  Serial2.println("AT+CMQTTSUB=0,4,1,1");
  delay(20);
  Serial2.println("HAII");
  return true;
}
/*
void publishMessage(const String &topic, const String &message)
{
  int topicLength = topic.length();
  int messageLength = message.length();
  Serial2.println("AT+CMQTTTOPIC=0," + String(topicLength));
  delay(20);
  Serial2.println(topic);
  delay(20);
  Serial2.println("AT+CMQTTPAYLOAD=0," + String(messageLength));
  delay(20);
  Serial2.println(message);
  delay(20);
  Serial2.println("AT+CMQTTPUB=0,1,60"); // QoS = 1, Retain = 0
  delay(20);
}*/

bool initSucess = false;
void initSim(String apn, String gprsUser, String gprsPass)
{  
    pinMode(MOSFET_SIM, OUTPUT);
    digitalWrite(MOSFET_SIM, HIGH); 
    Serial2.begin(BAUD, SERIAL_8N1, RXD2_SIMX, TXD2_SIMX);    
    if (sendATCommand("AT", "OK", 10, 5000)) {
        Serial.println("Modem ok");
        //return;
    }
    getIMEI();
    if(!setupAPN(apn,gprsUser,gprsPass)){
      Serial.println("Apn ok");
    } 
      
    if(!connectToServer("elementz/123", "64.226.117.238", 1883)){
      Serial.println("MQTT ok");
      subscribeToTopic("led/subscribe");
    }
    
    /*else{
      
      subscribeToTopic("test/test");
      
      }*/

    initSucess =true;
        
}

int cont=0;
void simloop(){ 
  if(initSucess){    
   if(sendATCommand("AT", "OK", 1, 50)){
      if(sendATCommand("AT+CMQTTCONNECT?", "OK", 1, 50)){
       Serial.println("MQTT OK");    
    //   publishMessage("test/qw",String(cont++));
      }else{
       Serial.println("MQTT lost");  
      }
   }    
   delay(5000);
  }
}
