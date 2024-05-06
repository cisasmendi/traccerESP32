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
    while (SerialGps.available() > 0)
    {
       
        if (gps.encode(SerialGps.read()))
        {
            if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid())
            {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
                date = gps.date.day();
                month = gps.date.month();
                year = gps.date.year();
                hour = gps.time.hour();
                minute = gps.time.minute();
                second = gps.time.second();
                valid = true;
            }
            else
            {
                valid = false;
            }        
        }
    }
    String result = "\"date_time\":null,\"lat\":null,\"lon:\"null";
    if (valid)
    {
        date_str = String(year) + "-" + String(month) + "-" + String(date);
        time_str = String(hour) + ":" + String(minute) + ":" + String(second);
        lat_str = String(latitude, 6);
        lng_str = String(longitude, 6);
        result = "\"date_time\":" + date_str + " " + time_str + ",\"lat\":" + lat_str + ",\"lon\":" + lng_str;
    }
    return result;
}
