#include "ControllerPreferences.h"
#include <WiFi.h>

void initWiFi()
{
 
 // saveWiFiCredentials("Cisasmendi88.4G", "Tiziana2285");

  // Cargar y conectar
  String ssid, password;
  if (loadWiFiCredentials(ssid, password))
  {
    Serial.println("Conectando a WiFi con credenciales guardadas...");
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("¡Conectado!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    // Desconectar y apagar WiFi
    WiFi.disconnect(true);
    Serial.println("WiFi desconectado y módulo apagado");
  }
  else
  {
    Serial.println("No se encontraron credenciales guardadas.");
  }
}
