#ifndef ESP32_PORTAL_PORTALFRAMEWORK_H
#define ESP32_PORTAL_PORTALFRAMEWORK_H

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

static const std::string TagSecret = "$1$gJvI";

#define ADMIN_USER_ID 999

typedef portal_PlayerData PlayerData;

class PortalFramework {

public:
    bool begin();

    void addOnConnectCallback(const std::function<void(PlayerData)> &callback) { tagConnectedCallbacks.push_back(callback); }

    void addOnDisconnectCallback(const std::function<void(void)> &callback) { reader.addOnDisconnectCallback(callback); }

    void addErrorCallback(const std::function<void(const String *)> &callback) { errorCallbacks.push_back(callback); }

    bool isTagConnected() const { return reader.isTagConnected(); }

    bool initializeTag();

    bool writePlayerData(PlayerData &playerData);

private:
    bool readPlayerData(PlayerData *playerData);

    bool write(byte *data, int size);

    bool read(byte *byte, int size);

    void handleConnectedTag(const String &uid);

    void onErrorMessage(const String *text) {
        for (auto &callback: errorCallbacks) callback(text);
    }

    MFRCTagReader reader = MFRCTagReader(&Debug, NFC_PIN_SS, NFC_PIN_RST);

    std::vector<std::function<void(PlayerData playerData)>> tagConnectedCallbacks;
    std::vector<std::function<void(const String *)>> errorCallbacks;
};


#endif //ESP32_PORTAL_PORTALFRAMEWORK_H
