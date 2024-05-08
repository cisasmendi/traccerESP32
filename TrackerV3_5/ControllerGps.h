#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define RX_GPS 14
#define TX_GPS 12
#define ON_OFF_GPS 22
TinyGPSPlus gps;
SoftwareSerial SerialGps(RX_GPS, TX_GPS);
float latitude, longitude;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str;
int pm;

void initGps()
{
  pinMode(ON_OFF_GPS, OUTPUT);
  digitalWrite(ON_OFF_GPS, LOW);
  SerialGps.begin(9600);
}

bool valid = false;
String getGpsData()
{
  String result = "\"date_time\":null,\"lat\":null,\"lon:\"null";
  date_str = "";
  time_str = "";
  while (SerialGps.available() > 0)
  {
    char c = SerialGps.read(); // Read a character from the GPS
    Serial.println(c);           // Print the character to the Serial monitor
    if (gps.encode(c))
    {
      if (gps.location.isValid() /*&& gps.date.isValid() && gps.time.isValid()*/)
      {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        lat_str = String(latitude, 6);
        lng_str = String(longitude, 6);                 
      }
      if (gps.date.isValid())
      {
        
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date); // values of date,month and year are stored in a string
        date_str += "/";

        if (month < 10)
          date_str += '0';
        date_str += String(month); // values of date,month and year are stored in a string
        date_str += "/";
        if (year < 10)
          date_str += '0';
        date_str += String(year); // values of date,month and year are stored in a string

      }
      if (gps.time.isValid()) // check whether gps time is valid
      {      
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30); // converting to IST
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5);
        if (hour > 23)
          hour = hour - 24; // converting to IST
        if (hour >= 12)     // checking whether AM or PM
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour); // values of hour,minute and time are stored in a string
        time_str += ":";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute); // values of hour,minute and time are stored in a string
        time_str += ":";
        if (second < 10)
          time_str += '0';
        time_str += String(second); // values of hour,minute and time are stored in a string
        if (pm == 1)
          time_str += "PM";
        else
          time_str += "AM";
      }
    }
  }
  result = "\"date_time\":\"" + date_str + ":" + time_str + "\",\"lat\":" + lat_str + ",\"lon\":" + lng_str;
  return result;
}
