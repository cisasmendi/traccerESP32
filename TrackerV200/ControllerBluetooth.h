

// retorna el valor de la mac del  bluetooth del esp32

String getBluetoothMac()
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  String mac = "";
  for (int i = 0; i < 5; i++)
  {
    mac += String(baseMac[i], HEX) + ":";
  }
  mac += String(baseMac[5], HEX);
  return mac;
}
