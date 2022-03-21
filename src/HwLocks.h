#ifndef ESP32_NFC_HWLOCKS_H
#define ESP32_NFC_HWLOCKS_H

#include <mutex>

class HwLocks {
public:
    static std::mutex I2C;
    static std::mutex I2S;
    static std::mutex SPI;
};

#endif //ESP32_NFC_HWLOCKS_H
