#ifndef ESP32_PORTAL_DEFS_H
#define ESP32_PORTAL_DEFS_H

#include <types.h>

struct Transaction {
    u64 time;
    u8 user_id;
    i8 strength;
    i8 magic = 0;
    i8 dexterity = 0;
    i8 bonus_points = 0;
    i8 skill = 0;
};

#endif //ESP32_PORTAL_DEFS_H
