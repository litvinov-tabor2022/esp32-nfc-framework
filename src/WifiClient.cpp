#include <WiFi.h>
#include <types.h>
#include "debugging.h"
#include "WifiClient.h"
#include "Tasker.h"

bool WifiClient::connect(const char *ssid, const char *password, const u32 timeoutMs) {
    WiFiClass::mode(WIFI_MODE_STA);
    WiFi.disconnect(false, true);
    WiFi.softAPdisconnect(false);

    Serial.printf("Connecting to wifi %s with timeout %d ms...\n", ssid, timeoutMs);
    WiFi.begin(ssid, password);

    const u64 start = millis();
    u64 now;
    while (WiFiClass::status() != WL_CONNECTED && ((now = millis()) - start) < timeoutMs) {
        Tasker::sleep(100);
    }

    Debug.printf("Connected to wifi %s with IP %s!\n", ssid, WiFi.localIP().toString().c_str());

    return WiFiClass::status() == WL_CONNECTED;
}

bool WifiClient::disconnect() {
    return WiFi.disconnect(false, true);
}
