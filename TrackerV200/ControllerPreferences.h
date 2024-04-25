#include <Preferences.h>
Preferences preferences;

void saveWiFiCredentials(const char* ssid, const char* password) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
}

bool loadWiFiCredentials(String &ssid, String &password) {
    preferences.begin("wifi", true);
    if (!preferences.isKey("ssid") || !preferences.isKey("password")) {
        preferences.end();
        return false;
    }
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();
    return true;
}
