#include "ControllerSim7600.h"
// https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/module/sim7600x/SIM7500_SIM7600_SIM7800%20Series_MQTT_Application%20Note_V2.00.pdf

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("Wait...");
  initSim("igprs.claro.com.ar", "", "");
}

void loop()
{

  simloop();
}
