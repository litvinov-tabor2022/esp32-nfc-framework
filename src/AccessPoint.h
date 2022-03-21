#ifndef ESP32NFC_ACCESSPOINT
#define ESP32NFC_ACCESSPOINT

#include <debugging.h>
#include <Tasker.h>
#include <DNSServer.h>

class AccessPoint {
public:
    static void start(const char *ssid, const char *password);
    static void stop();
};

#endif //ESP32NFC_ACCESSPOINT
