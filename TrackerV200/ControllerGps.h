#include <TinyGPS++.h> 
#include <SoftwareSerial.h>

#define RX_GPS 14
#define TX_GPS 12
#define ON_OFF_GPS 22
TinyGPSPlus gps; 
SoftwareSerial SerialGps(RX_GPS, TX_GPS);
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;

void initGps(){
    pinMode(ON_OFF_GPS,OUTPUT);
    digitalWrite(ON_OFF_GPS, HIGH);
    SerialGps.begin(9600);
}

String getGpsData(){
    while (SerialGps.available() > 0){
        if (gps.encode(SerialGps.read())){
            if (gps.location.isValid()){
                latitude = gps.location.lat();
                longitude = gps.location.lng();
            }
            if (gps.date.isValid()){
                date = gps.date.day();
                month = gps.date.month();
                year = gps.date.year();
            }
            if (gps.time.isValid()){
                hour = gps.time.hour();
                minute = gps.time.minute();
                second = gps.time.second();
            }
        }
    }
    date_str = String(year) + "-" + String(month) + "-" + String(date);
    time_str = String(hour) + ":" + String(minute) + ":" + String(second);
    lat_str = String(latitude, 6);
    lng_str = String(longitude, 6);
    return date_str + " " + time_str + " " + lat_str + " " + lng_str;
}
