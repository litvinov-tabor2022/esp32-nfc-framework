#include <WiFi.h>
#include <AccessPoint.h>

void AccessPoint::start(const char *ssid, const char *password) {
    WiFi.disconnect(true); // for case it was configured as wifi client in last run...

    IPAddress apIP(192, 168, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);

    Debug.print(F("Configuring access point...\n"));
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(apIP, apIP, netMsk);

    Tasker::sleep(500);
    Debug.println("AP ready");
}

void AccessPoint::stop() {
    WiFi.softAPdisconnect(true);
    Debug.println("AP stopped");
}
