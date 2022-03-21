#ifndef ESP_TASKER_H
#define ESP_TASKER_H

#include <functional>
#include <vector>

class Tasker {
public:
    explicit Tasker(int core);

    void once(const char *name, std::function<void(void)> op) const;

    void loop(const char *name, std::function<void(void)> op) const;

    void loopEvery(const char *name, int millis, std::function<void(void)> op) const;

    static void sleep(int millis);
    static void yield();

private:
    int core;
};

extern Tasker Core1;
extern Tasker Core0;

#endif //ESP_TASKER_H
