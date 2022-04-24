#include <WiFi.h>
#include <AccessPoint.h>

bool AccessPoint::start(const char *ssid, const char *password) {
    WiFi.disconnect(false, true); // for case it was configured as wifi client in last run...
    WiFiClass::mode(WIFI_MODE_AP);

    IPAddress apIP(192, 168, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);

    Debug.print(F("Configuring access point...\n"));
    WiFi.softAP(ssid, password);
    if (!WiFi.softAPConfig(apIP, apIP, netMsk)) {
        Debug.println("Could not configure AP");
        return false;
    }

    Tasker::sleep(500);
    Debug.println("AP ready");

    return true;
}

bool AccessPoint::stop() {
    Debug.println("AP stopping");
    WiFi.disconnect(false, true); // can fail

    return WiFi.softAPdisconnect(false);
}
