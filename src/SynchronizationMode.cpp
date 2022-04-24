#include "SynchronizationMode.h"
#include <types.h>
#include <AccessPoint.h>
#include <WifiClient.h>
#include <PortalFramework.h>

static const u32 timeout = 10000;

bool SynchronizationMode::start() {
    std::lock_guard<std::mutex> lg(switchMutex);
    started = true;

    webServer->start();
    return AccessPoint::stop() &&
           WifiClient::connect(this->frameworkConfig->syncSSID.c_str(), this->frameworkConfig->syncPass.c_str(), timeout);
}

bool SynchronizationMode::stop() {
    std::lock_guard<std::mutex> lg(switchMutex);
    started = false;

    webServer->stop();
    return WifiClient::disconnect() && AccessPoint::start(this->deviceConfig->apSSID.c_str(), this->deviceConfig->apPass.c_str());
}

SynchronizationMode::SynchronizationMode(WebServer *webServer, FrameworkConfig *frameworkConfig, DeviceConfig *deviceConfig) :
        webServer(webServer), frameworkConfig(frameworkConfig), deviceConfig(deviceConfig) {}

bool SynchronizationMode::toggle() {
    if (started) { return stop(); } else { return start(); }
}
