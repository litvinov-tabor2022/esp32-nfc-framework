#include "Clocks.h"
#include "HwLocks.h"
#include "Tasker.h"

u64 Clocks::getCurrentTime() {
    std::lock_guard<std::mutex> lg(HwLocks::I2C);
    auto time = rtc.now();
    u64 timeUnix = time.unixtime();

    while (time.year() != 2022 || time.month() != 7 || timeUnix <= lastTimestamp) {
        Debug.printf("Invalid timestamp returned (%llu), retrying\n", timeUnix);
        time = rtc.now();
        timeUnix = time.unixtime();
        Tasker::sleep(100);
    }

    lastTimestamp = timeUnix;
    return timeUnix;
}

bool Clocks::begin() {
    {
        std::lock_guard<std::mutex> lg(HwLocks::I2C);
        if (!rtc.begin()) return false;
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
