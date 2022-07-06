#include <PortalFramework.h>
#include <MFRCTagReader.h>
#include <utils.h>

#include <pb_decode.h>
#include <pb_encode.h>

#include <utility>

#define BUFFER_SIZE 32 // don't have this uselessly big
byte rawTagData[BUFFER_SIZE];

std::optional<std::string> PortalFramework::begin(bool ignoreRTCFailure, bool supportSyncMode) {
    if (!storage.begin()) {
        Debug.println("Could not initialize transactions log!");
        return std::optional("Could not initialize transactions log!");
    }

    if (!resources.begin()) {
        Debug.println("Could not load resources!");
        return std::optional("Could not load resources!");
    }

    if (!loadDeviceConfig()) {
        Debug.println("Could not load device config!");
        return std::optional("Could not load device config!");
    }

    if (!loadFrameworkConfig()) {
        Debug.println("Could not load framework config!");
        return std::optional("Could not load framework config!");
    }

    {
        std::lock_guard<std::mutex> lg(HwLocks::SPI);

        reader = new MFRCTagReader(&Debug, NFC_PIN_SS, NFC_PIN_RST);

        if (!reader->begin()) {
            Debug.println("Could not initialize tag reader!");
            return std::optional("Could not initialize tag reader!");
        }
    }

    if (!clocks.begin()) {
        if (ignoreRTCFailure) {
            Debug.println("RTC init failed but it's set to be ignored");
            rtcFailed = true;
        } else {
            Debug.println("Could not initialize RTC!");
            return std::optional("Could not initialize RTC!");
        }
    }

    reader->addOnConnectCallback([this](const byte *uid) {
        const String uidStr = hexStr(uid, MFRC_UID_LENGTH);

        if (reader->isTagConnected()) {
            Core0.once("handleConnTag", [this, uidStr]() {
                handleConnectedTag(uidStr);
            });
        }
    });

    Core1.loopEvery("rfid", 50, [this] {
        std::lock_guard<std::mutex> lg(HwLocks::SPI);
        reader->checkTagPresented();
    });

    if (supportSyncMode) {
        if (!ap.start(deviceConfig.apSSID.c_str(), deviceConfig.apPass.c_str())) {
            Debug.println("Could not start AP!");
            return std::optional("Could not start AP!");
        }
    } else {
        Debug.println("Synchronization mode is turned off, NOT starting wifi");
    }

    return std::nullopt;
}

void PortalFramework::handleConnectedTag(const String &uid) {
    Debug.println("--------------------------");
    Debug.printf("Handling new connected tag, UID %s\n", uid.c_str());

    PlayerData playerData = portal_PlayerData_init_zero;
    if (readPlayerData(&playerData)) {
        if (reader->isTagConnected()) {
            for (auto &callback: tagConnectedCallbacks) callback(playerData, false);
        } else {
            Debug.println("Tag disconnected during reading, quit");
            return;
        }
    } else {
        Debug.println("Tag can't be read, quit");
        onErrorMessage(new String("Chyba cteni tagu!"));
        return;
    }
}

bool PortalFramework::initializeTag() {
    PlayerData playerData = portal_PlayerData_init_zero;

    Serial.println("--------------------------------------------------------------------");
    Debug.println("Initializing tag to default values");

    strcpy(playerData.secret, TagSecret.c_str());
    do {
        playerData.user_id = esp_random() % 0xff; // random ID
    } while (playerData.user_id == ADMIN_USER_ID); // ensure it doesn't collide

    playerData.strength = 0;
    playerData.magic = 0;
    playerData.dexterity = 0;

    if (!writePlayerData(playerData)) {
        Debug.println("Can't initialize the tag!");
        return false;
    }

    return true;
}

bool PortalFramework::writePlayerData(_portal_PlayerData &playerData) {
    pb_ostream_t os = pb_ostream_from_buffer(rawTagData, BUFFER_SIZE);

    if (!pb_encode_delimited(&os, portal_PlayerData_fields, &playerData)) {
        Debug.printf("Encoding player data failed: %s\n", PB_GET_ERROR(&os));
        return false;
    }

    const u8 size = os.bytes_written;

    if (size > BUFFER_SIZE) {
        Debug.println("!!!\n!!!\nWriting more data than fits the buffer!!!\n!!!\n!!!");
    }

    if (!nfcWrite(rawTagData, size)) {
        Debug.println("Could not write data to tag!");
        return false;
    }

    // update data everywhere!
    for (auto &callback: tagConnectedCallbacks) callback(playerData, true);

    return true;
}

bool PortalFramework::readPlayerData(_portal_PlayerData *playerData) {
    if (!nfcRead(rawTagData, BUFFER_SIZE)) {
        Debug.println("Can't read data from tag or not all data was read");
        return false;
    }

    pb_istream_t stream = pb_istream_from_buffer(rawTagData, BUFFER_SIZE);

    if (!pb_decode_delimited(&stream, portal_PlayerData_fields, playerData)) {
        Debug.printf("Decoding player data failed: %s\n", PB_GET_ERROR(&stream));
        return false;
    }

    // check secret, so we can be sure it's really our tag...
    if (strcmp(TagSecret.c_str(), playerData->secret) != 0) {
        Debug.println("Tag secret doesn't match!");
        return false;
    }

    return true;
}

bool PortalFramework::nfcWrite(byte *data, int size) {
    std::lock_guard<std::mutex> lg(HwLocks::SPI);
    return reader->write(data, size);
}

bool PortalFramework::nfcRead(byte *byte, int size) {
    std::lock_guard<std::mutex> lg(HwLocks::SPI);
    return reader->read(byte, size);
}

bool PortalFramework::loadFrameworkConfig() {
    const auto doc = storage.loadJsonFile(PATH_CONFIG_FRAMEWORK);
    if (doc == nullptr) {
        Debug.println("Could not load framework config resource");
        return false;
    }

    const auto c = doc->as<JsonObject>();

    frameworkConfig = FrameworkConfig{
            .syncSSID = c["syncSSID"],
            .syncPass = c["syncPass"],
    };

    return !(frameworkConfig.syncPass.isEmpty() || frameworkConfig.syncSSID.isEmpty());
}

bool PortalFramework::loadDeviceConfig() {
    const auto doc = storage.loadJsonFile(PATH_CONFIG_DEVICE);
    if (doc == nullptr) {
        Debug.println("Could not load device config resource");
        return false;
    }

    const auto c = doc->as<JsonObject>();

    deviceConfig = DeviceConfig{
            .deviceId = c["deviceId"],
            .apSSID = c["apSSID"],
            .apPass = c["apPass"],
    };

    return !(deviceConfig.apPass.isEmpty() || deviceConfig.apSSID.isEmpty() || deviceConfig.deviceId.isEmpty());
}

u64 PortalFramework::getCurrentTime() {
    if (rtcFailed) {
        Debug.println("Could not RTC module as it's not initialized!!! Will reset the ESP...");
        esp_restart();
    }
    return clocks.getCurrentTime();
}

void PortalFramework::setCurrentTime(const u64 unixSecs) {
    if (rtcFailed) {
        Debug.println("Could not RTC module as it's not initialized!!! Will reset the ESP...");
        esp_restart();
    }
    clocks.setCurrentTime(unixSecs);
}
