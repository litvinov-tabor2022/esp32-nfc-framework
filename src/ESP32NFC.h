#ifndef ESP32_NFC_FRAMEWORK_ESP32NFC_H
#define ESP32_NFC_FRAMEWORK_ESP32NFC_H

#include <types.h>
#include <Tasker.h>
#include <debugging.h>
#include <HwLocks.h>
#include <AccessPoint.h>

#define MFRC_RESET_TIMEOUT 500
#define NFC_PIN_SS 32
#define NFC_PIN_RST 25

#include <MFRCTagReader.h>

class ESP32NFC {

public:
    bool begin(const char* ssid, const char* password);

    void addOnConnectCallback(const std::function<void(byte *)> &callback) {
        reader.addOnConnectCallback(callback);
    }

    void addOnDisconnectCallback(const std::function<void(void)> &callback) {
        reader.addOnDisconnectCallback(callback);
    }

    bool isTagConnected() const { return reader.isTagConnected(); }

    bool write(byte *data, int size);

    bool read(byte *byte, int size);

private:
    MFRCTagReader reader = MFRCTagReader(&Debug, NFC_PIN_SS, NFC_PIN_RST);
};


#endif //ESP32_NFC_FRAMEWORK_ESP32NFC_H
