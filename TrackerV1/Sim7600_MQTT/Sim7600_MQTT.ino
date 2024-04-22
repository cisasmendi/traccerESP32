/*
Developed by Nandu, Anandu, Unnikrishnan 
Company: Elementz Engineers Guild Pvt Ltd
*/

#define sw 4
#define led 2

#define RXD2 16
#define TXD2 17
#define MOSFET_SIM 19 
#define ON_OFF_GPS 22

int flag = 1;
int flag1 = 0;
int state=0;
String Publish = "led/publish"; //Publish Topic
String Subscribe = "led/subscribe"; //Subscribe Topic

bool reply = false;
void setup() 
{

 pinMode(MOSFET_SIM, OUTPUT); 
 pinMode(ON_OFF_GPS,OUTPUT);
 digitalWrite(MOSFET_SIM, HIGH);
 digitalWrite(ON_OFF_GPS, HIGH);
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  pinMode(sw, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  int i = 10;
  Serial.println("\nTesting Modem Response...\n");
  Serial.println("****");
   while (i) {
        Serial2.println("AT");
        delay(500);
        if (Serial2.available()) {
            String r = Serial2.readString();
            Serial.println(r);
            if ( r.indexOf("OK") >= 0 ) {
                reply = true;
                break;;
            }
        }
        delay(500);
        i--;
    }

  //AT Commands for setting up the client id and Server
  //Need to be executed once -- Open serial terminal doe seeing the debug messages
  Serial.println("Connecting To Server........");
  Serial2.println("ATE0");
  delay(2000);
  Serial2.println("AT+CMQTTSTART"); //Establishing MQTT Connection
  delay(2000); 
  Serial2.println("AT+CMQTTACCQ=0,\"elementz/123\""); //Client ID - change this for each client as this need to be unique
  delay(2000);
  Serial2.println("AT+CMQTTCONNECT=0,\"tcp://64.226.117.238:1883\",90,1"); //MQTT Server Name for connecting this client
  delay(2000);

  //SUBSCRIBE MESSAGE
  //Need to be executed once
  Serial2.println("AT+CMQTTSUBTOPIC=0,9,1"); //AT Command for Setting up the Subscribe Topic Name 
  delay(2000);
  Serial2.println(Subscribe); //Topic Name
  delay(2000);
  Serial2.println("AT+CMQTTSUB=0,4,1,1"); //Length of message
  delay(2000);
  Serial2.println("HAII"); //message
  delay(2000);
  Serial.println("Done");
}

void loop() 
{
  String a;
  if(state==0)
  {

    if(digitalRead(sw) == 0 && flag1 == 0)
    {
      //PUBLISH MESSAGE
      flag1 = 1;
      digitalWrite(led, HIGH);
      Serial.println("Publishing Message: LED ON");
      Serial2.println("AT+CMQTTTOPIC=0,8"); //AT Command for Setting up the Publish Topic Name
      delay(1000);
      Serial2.println(Publish); //Topic Name
      delay(1000);
      Serial2.println("AT+CMQTTPAYLOAD=0,1"); //Payload length
      delay(1000);
      Serial2.println("a"); //Payload message
      delay(1000);
      Serial2.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
      delay(1000);
    }
    else if(digitalRead(sw) == 0 && flag1 == 1)
    {
      flag1 = 0;
     digitalWrite(led, LOW); 
      Serial.println("Publishing Message: LED OFF");
      Serial2.println("AT+CMQTTTOPIC=0,8"); //AT Command for Setting up the Publish Topic Name
      delay(1000);
      Serial2.println(Publish); //Topic Name
      delay(1000);
      Serial2.println("AT+CMQTTPAYLOAD=0,1"); //Payload length
      delay(1000);
      Serial2.println("b"); //Payload message
      delay(1000);
      Serial2.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
      delay(1000);
    }
  }
  if(state==1)
  {
    if(digitalRead(sw) == 0 && flag1 == 0)
    {
      //PUBLISH MESSAGE
      flag1 = 1;
      digitalWrite(led, LOW);
      Serial.println("Publishing Message: LED OFF");
      Serial2.println("AT+CMQTTTOPIC=0,8"); //AT Command for Setting up the Publish Topic Name
      delay(1000);
      Serial2.println(Publish); //Topic Name
      delay(1000);
      Serial2.println("AT+CMQTTPAYLOAD=0,1"); //Payload length
      delay(1000);
      Serial2.println("b"); //Payload message
      delay(1000);
      Serial2.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
      delay(1000);
    }
    else if(digitalRead(sw) == 0 && flag1 == 1)
    {
      flag1 = 0;
      digitalWrite(led,HIGH); 
      Serial.println("Publishing Message: LED ON");
      Serial2.println("AT+CMQTTTOPIC=0,8"); //AT Command for Setting up the Publish Topic Name
      delay(1000);
      Serial2.println(Publish); //Topic Name
      delay(1000);
      Serial2.println("AT+CMQTTPAYLOAD=0,1"); //Payload length
      delay(1000);
      Serial2.println("a"); //Payload message
      delay(1000);
      Serial2.println("AT+CMQTTPUB=0,1,60"); //Acknowledgment
      delay(1000);
    }
  }

 //Receiving MODEM Response
  while(Serial2.available()>0)
  {
    delay(10);
    a = Serial2.readString();
    if(flag==0)
    {
      //Serial.println(a);
    flag = 1;
    }
    //Serial.println(b);
    if(a.indexOf("PAYLOAD") != -1)
    {
       flag = 0;
       int new1 = a.indexOf("PAYLOAD");
       String neww = a.substring(new1);
       int new2 = neww.indexOf('\n');
       String new3 = neww.substring(new2+1);
       int new4 = new3.indexOf('\n');
       String new5 = new3.substring(0,new4);
       
       Serial.println("Topic: led/subscribe");
       Serial.print("Message is: ");
       Serial.println(new5);
       new5.remove(new5.length()-1);
       if(new5 == "a")
       {
        state=1;
        Serial.println("LED ON");
        digitalWrite(led, HIGH);
       }
       else if(new5 == "b")
       {
        state=0;
        flag1=0;
        Serial.println("LED OFF");
        digitalWrite(led, LOW);
       }
    }      
  }
}
