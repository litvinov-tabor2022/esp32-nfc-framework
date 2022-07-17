#ifndef ESP32_PORTAL_CLOCKS_H
#define ESP32_PORTAL_CLOCKS_H

#include "RTClib.h"

#include "debugging.h"
#include "types.h"

class Clocks {
public:
    bool begin();
    u64 getCurrentTime();
    void setCurrentTime(u64 unixSecs);
private:
    void printCurrentTime();

    u64 lastTimestamp = 0;

    RTC_DS1307 rtc;
};


#endif //ESP32_PORTAL_CLOCKS_H
