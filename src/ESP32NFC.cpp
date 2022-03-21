

#include "ESP32NFC.h"

bool ESP32NFC::begin(const char *ssid, const char *password) {
    if (!reader.begin()) {
        Debug.println("Could not initialize tag reader!");
        return false;
    }

    Core0.once("AP start", [ssid, password] {
        Debug.println("Starting AP...");
        AccessPoint::start(ssid, password);
        TelnetPrint.begin();
    });

    Core1.loopEvery("rfid", 50, [this] {
        std::lock_guard<std::mutex> lg(HwLocks::SPI);
        reader.checkTagPresented();
    });

    return true;
}

bool ESP32NFC::write(byte *data, int size) {
    std::lock_guard<std::mutex> lg(HwLocks::SPI);
    return reader.write(data, size);
}

bool ESP32NFC::read(byte *byte, int size) {
    std::lock_guard<std::mutex> lg(HwLocks::SPI);
    return reader.read(byte, size);
}
