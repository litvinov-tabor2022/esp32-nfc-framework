#ifndef ESP32_PORTAL_DEFS_H
#define ESP32_PORTAL_DEFS_H

#include <types.h>

#define PATH_CONFIG_FRAMEWORK "/configs/framework.json"
#define PATH_CONFIG_DEVICE "/configs/device.json"
#define PATH_NAMES "/resources/names.json"
#define PATH_PRICELIST "/resources/prices.json"

struct Transaction {
    u64 time;
    String device_id;
    u16 user_id;
    i8 strength = 0;
    i8 magic = 0;
    i8 dexterity = 0;
    i16 skill = 0;
    i8 bonus_points = 0;
};

#endif //ESP32_PORTAL_DEFS_H
