#include <WiFi.h>
#include <WiFiServer.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> 


#define PIN_LED_R 26
#define MOSFET_SIM 19
#define ON_OFF_GPS 22
#define RX_GPS 14
#define TX_GPS 12
#define vel 200

const char* ssid = "Cisasmendi88.4G"; //ssid of your wifi
const char* password = "Tiziana2285"; //password of your wifi
TinyGPSPlus gps; 

SoftwareSerial ss(RX_GPS, TX_GPS);
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;

WiFiServer server(80);

void setup(){  
 pinMode(PIN_LED_R, OUTPUT);
 pinMode(MOSFET_SIM, OUTPUT);   
 pinMode(ON_OFF_GPS,OUTPUT);
 digitalWrite(MOSFET_SIM, HIGH);
 digitalWrite(ON_OFF_GPS, HIGH);

  Serial.begin(115200);
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //connecting to wifi
  while (WiFi.status() != WL_CONNECTED)// while wifi not connected
  {
    delay(500);
    Serial.print("."); //print "...."
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());  // Print the IP address
}

void parpadear(){
    // Parpadear el LED conectado al pin 2
  digitalWrite(PIN_LED_R, HIGH); // Encender el LED
  delay(vel); // Esperar medio segundo
  digitalWrite(PIN_LED_R, LOW); // Apagar el LED
  delay(vel); // Esperar medio segundo
}

void loop()
{
   parpadear();
  while (ss.available() > 0) {//while data is available
   char c = ss.read();  // Read a character from the GPS
       Serial.print(c);     // Print the character to the Serial monitor
       
    if (gps.encode(c)) //read gps data
    {
     
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string
       // Serial.print(c);
      }
      if (gps.date.isValid()) //check whether gps date is valid
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);// values of date,month and year are stored in a string
        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month); // values of date,month and year are stored in a string
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year); // values of date,month and year are stored in a string
      }
      if (gps.time.isValid())  //check whether gps time is valid
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30); // converting to IST
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;   // converting to IST
        if (hour >= 12)  // checking whether AM or PM
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second); //values of hour,minute and time are stored in a string
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
    }
}
 WiFiClient client = server.available(); // Check if a client has connected
  if (!client)
  {
    return;
  }
  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS DATA</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> GPS DATA</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td>  </tr> </table> ";

  s += "</body> </html>";

  client.print(s); // all the values are send to the webpage
  delay(100);
}
