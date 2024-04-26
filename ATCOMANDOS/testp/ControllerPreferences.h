#include <Preferences.h>
Preferences preferences;

void resetConfig(){
   preferences.begin("config", false);
   preferences.clear();
   preferences.end();
}

// getConfig
String getConfig(bool &wifienable, String &ssid, String &password, String &secret, String &broker, int &port, String &topic, String &apn, String &gprsUser, String &gprsPass, bool &reset){
    String result = "";
    preferences.begin("config", true);
    wifienable = preferences.getBool("wifienable",true);
    ssid = preferences.getString("ssid1", "redlocal");
    password = preferences.getString("password1", "redlocal");
    secret = preferences.getString("secret", "123456789");
    broker = preferences.getString("broker", "192.168.1.100");
    port = preferences.getInt("port", 1883);
    topic = preferences.getString("topic", "system");
    apn = preferences.getString("apn", "");
    gprsUser = preferences.getString("gprsUser", "");
    gprsPass = preferences.getString("gprsPass", "");
    reset = preferences.getBool("reset", false);    
    result = "wifienable: " + String(wifienable) + " ssid: " + ssid + " password: " + password + " secret: " + secret + " broker: " + broker + " port: " + String(port) + " topic: " + topic + " apn: " + apn + " gprsUser: " + gprsUser + " gprsPass: " + gprsPass + " reset: " + String(reset);
    preferences.end();
    return result;
}

void setWiFi(String ssid, String pass) {
    preferences.begin("config", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", pass);
    preferences.end();
}

void getWiFi(String &ssid, String &pass) {
    preferences.begin("config", true);  
    ssid = preferences.getString("ssid", "redlocal");
    pass = preferences.getString("password", "redlocal");
    preferences.end();  
}


// set config apn
void setConfigApn(String apn, String gprsUser, String gprsPass){
    preferences.begin("config", false);
    preferences.putString("apn", apn);
    preferences.putString("gprsUser", gprsUser);
    preferences.putString("gprsPass", gprsPass);
    preferences.end();
}

// get config apn
void getConfigApn(String &apn, String &gprsUser, String &gprsPass){
    preferences.begin("config", true);
    apn = preferences.getString("apn", "");
    gprsUser = preferences.getString("gprsUser", "");
    gprsPass = preferences.getString("gprsPass", "");
    preferences.end();
}

// set config mqtt
void setConfigMqtt(String broker, int port, String topic){
    preferences.begin("config", false);
    preferences.putString("broker", broker);
    preferences.putInt("port", port);
    preferences.putString("topic", topic);
    preferences.end();
}

// get config mqtt
void getConfigMqtt(String &broker, int &port, String &topic){
    preferences.begin("config", true);
    broker = preferences.getString("broker", "192.168.1.100");
    port = preferences.getInt("port", 1883);
    topic = preferences.getString("topic", "system");
    preferences.end();
}

// set config system
void setKeyLock(String secret){
    preferences.begin("config", false);
    preferences.putString("secret", secret);
    preferences.end();
}

// get config system
void getKeyLock(String &secret){
    preferences.begin("config", true);
    secret = preferences.getString("secret", "123456789");
    preferences.end();
}
