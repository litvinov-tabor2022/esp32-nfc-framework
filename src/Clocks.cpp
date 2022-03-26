#include "Clocks.h"
#include "HwLocks.h"

u64 Clocks::getCurrentTime() {
    std::lock_guard<std::mutex> lg(HwLocks::I2C);
    return rtc.now().unixtime();
}

bool Clocks::begin() {
    {
        std::lock_guard<std::mutex> lg(HwLocks::I2C);
        if (!rtc.begin()) return false;

        if (rtc.lostPower()) {
            Debug.println("RTC lost power, please update the time!");
        }
    }

    printCurrentTime();

    return true;
}

void Clocks::setCurrentTime(u64 unixSecs) {
    Debug.printf("Adjusting time to %llu secs\n", unixSecs);
    {
        std::lock_guard<std::mutex> lg(HwLocks::I2C);
        rtc.adjust(DateTime(unixSecs));
    }
    printCurrentTime();
}

void Clocks::printCurrentTime() {
    std::lock_guard<std::mutex> lg(HwLocks::I2C);

    Debug.print("Current time: ");
    char buf2[] = "DD/MM/YY hh:mm:ss UTC";
    Debug.print(rtc.now().toString(buf2));
    Debug.println();
}
