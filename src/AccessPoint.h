#ifndef ESP32NFC_ACCESSPOINT
#define ESP32NFC_ACCESSPOINT

#include <debugging.h>
#include <Tasker.h>
#include <DNSServer.h>
#include <mutex>

class AccessPoint {
public:
    bool start(const char *ssid, const char *password);
    bool stop();
private:
    std::mutex switchMutex;
    bool started = false;
};

#endif //ESP32NFC_ACCESSPOINT
