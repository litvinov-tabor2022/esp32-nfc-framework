#ifndef ESP32_PORTAL_PORTALFRAMEWORK_H
#define ESP32_PORTAL_PORTALFRAMEWORK_H

#include <optional>

#include <types.h>
#include <Tasker.h>
#include <debugging.h>
#include <HwLocks.h>
#include <AccessPoint.h>
#include <proto/portal.pb.h>

#define MFRC_RESET_TIMEOUT 500
#define NFC_PIN_SS 32
#define NFC_PIN_RST 25

#include <MFRCTagReader.h>
#include <Storage.h>
#include <Clocks.h>
#include <Resources.h>
#include "WebServer.h"
#include "SynchronizationMode.h"

static const std::string TagSecret = "$1$gJvI";

#define ADMIN_USER_ID 999

typedef portal_PlayerData PlayerData;

struct DeviceConfig {
    String deviceId;
    String apSSID;
    String apPass;
};

struct FrameworkConfig {
    String syncSSID;
    String syncPass;
};

class PortalFramework {

public:
    // None if no error
    std::optional<std::string> begin(bool ignoreRTCFailure = false);

    void addOnConnectCallback(const std::function<void(PlayerData, bool)> &callback) { tagConnectedCallbacks.push_back(callback); }

    void addOnDisconnectCallback(const std::function<void(void)> &callback) { reader->addOnDisconnectCallback(callback); }

    void addErrorCallback(const std::function<void(const String *)> &callback) { errorCallbacks.push_back(callback); }

    [[nodiscard]] bool isTagConnected() const { return reader->isTagConnected(); }

    bool initializeTag();

    bool writePlayerData(PlayerData &playerData);

    DeviceConfig &getDeviceConfig() { return deviceConfig; }

    Storage storage;
    Resources resources = Resources(&storage);
    SynchronizationMode synchronizationMode = SynchronizationMode(&webServer, &frameworkConfig, &deviceConfig);

    u64 getCurrentTime();
    void setCurrentTime(u64 unixSecs);

private:
    bool readPlayerData(PlayerData *playerData);

    bool nfcWrite(byte *data, int size);

    bool nfcRead(byte *byte, int size);

    void handleConnectedTag(const String &uid);

    bool loadFrameworkConfig();

    bool loadDeviceConfig();

    void onErrorMessage(const String *text) {
        for (auto &callback: errorCallbacks) callback(text);
    }

    MFRCTagReader *reader;
    Clocks clocks;
    WebServer webServer = WebServer(this);

    DeviceConfig deviceConfig;
    FrameworkConfig frameworkConfig;

    bool rtcFailed = false;

    std::vector<std::function<void(PlayerData playerData, bool isReload)>> tagConnectedCallbacks;
    std::vector<std::function<void(const String *)>> errorCallbacks;
};

#endif //ESP32_PORTAL_PORTALFRAMEWORK_H
