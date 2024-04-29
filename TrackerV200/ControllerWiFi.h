#include <WiFi.h>
void initWiFi(String ssid, String pass)
{ 
  //Serial.println(F("Conectando a WiFi con credenciales guardadas..."));
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  //Serial.print(F("Conectando a:\t"));
  //Serial.println(ssid); 
  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
  //Serial.print(F('.'));
  }
  // Mostrar mensaje de exito y dirección IP asignada
  //Serial.println();
  //Serial.print(F("Conectado a:\t"));
  //Serial.println(WiFi.SSID()); 
  //Serial.print(F("IP address:\t"));
  //Serial.println(WiFi.localIP());
  //Serial.println(F("WiFi desconectado y módulo apagado"));  
}
