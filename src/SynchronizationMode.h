#ifndef ESP32_NFC_FRAMEWORK_SYNCHRONIZATIONMODE_H
#define ESP32_NFC_FRAMEWORK_SYNCHRONIZATIONMODE_H

#include <mutex>
#include <WebServer.h>

struct DeviceConfig;
struct FrameworkConfig;

class SynchronizationMode {
public:
    explicit SynchronizationMode(WebServer *webServer, FrameworkConfig *frameworkConfig, DeviceConfig *deviceConfig);

    bool start();

    bool stop();

    bool toggle();

    [[nodiscard]] bool isStarted() const { return started; }

private:
    std::mutex switchMutex;

    bool started;

    WebServer *webServer;
    FrameworkConfig *frameworkConfig;
    DeviceConfig *deviceConfig;
};


#endif //ESP32_NFC_FRAMEWORK_SYNCHRONIZATIONMODE_H
