#ifndef ESP32_PORTAL_DEFS_H
#define ESP32_PORTAL_DEFS_H

#include <types.h>

enum Operation {
    ADD,
    REMOVE,
    UNKNOWN
};

struct Transaction {
    u64 time;
    u8 user_id;
    u8 strength;
    u8 magic = 0;
    u8 dexterity = 0;
    u16 skill = 0;
    u8 bonus_points = 0;
    Operation operation = UNKNOWN;
};

#endif //ESP32_PORTAL_DEFS_H
