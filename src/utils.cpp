#ifndef ESP32_PORTAL_UTILS_H
#define ESP32_PORTAL_UTILS_H

#include "utils.h"

#include "Arduino.h"

#include <sstream>
#include <iomanip>

const String hexStr(const byte *data, const int len) {
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
        ss << std::setw(2) << std::setfill('0') << (int) data[i];

    return {ss.str().c_str()};
}

#endif //ESP32_PORTAL_UTILS_H
