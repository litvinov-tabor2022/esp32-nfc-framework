#ifndef ESP32_PORTAL_DEFS_H
#define ESP32_PORTAL_DEFS_H

#include <types.h>

struct Transaction {
    u64 time;
    u8 user_id;
    i8 strength;
    i8 magic;
    i8 dexterity;
    i8 bonus_points;
};

#endif //ESP32_PORTAL_DEFS_H
