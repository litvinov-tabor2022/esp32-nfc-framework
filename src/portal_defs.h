#ifndef ESP32_PORTAL_DEFS_H
#define ESP32_PORTAL_DEFS_H

#include <types.h>

#define PATH_CONFIG_FRAMEWORK "/configs/framework.json"
#define PATH_CONFIG_DEVICE "/configs/device.json"
#define PATH_NAMES "/resources/names.json"
#define PATH_PRICELIST "/resources/prices.json"

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
