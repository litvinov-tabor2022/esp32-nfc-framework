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

static const std::string TagSecret = "$1$gJvI";

#define ADMIN_USER_ID 999

typedef portal_PlayerData PlayerData;

class PortalFramework {

public:
    // None if no error
    std::optional<std::string> begin();

    void addOnConnectCallback(const std::function<void(PlayerData)> &callback) { tagConnectedCallbacks.push_back(callback); }

    void addOnDisconnectCallback(const std::function<void(void)> &callback) { reader->addOnDisconnectCallback(callback); }

    void addErrorCallback(const std::function<void(const String *)> &callback) { errorCallbacks.push_back(callback); }

    [[nodiscard]] bool isTagConnected() const { return reader->isTagConnected(); }

    bool initializeTag();

    bool writePlayerData(PlayerData &playerData);

    Storage storage;
    Clocks clocks;
    Resources resources;
private:
    bool readPlayerData(PlayerData *playerData);

    bool nfcWrite(byte *data, int size);

    bool nfcRead(byte *byte, int size);

    void handleConnectedTag(const String &uid);

    void onErrorMessage(const String *text) {
        for (auto &callback: errorCallbacks) callback(text);
    }

    MFRCTagReader *reader;

    std::vector<std::function<void(PlayerData playerData)>> tagConnectedCallbacks;
    std::vector<std::function<void(const String *)>> errorCallbacks;
};


#endif //ESP32_PORTAL_PORTALFRAMEWORK_H
