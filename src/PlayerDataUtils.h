#ifndef ESP32_PORTAL_FRAMEWORK_PLAYERDATAUTILS_H
#define ESP32_PORTAL_FRAMEWORK_PLAYERDATAUTILS_H

#include "PortalFramework.h"
#include <set>

class PlayerDataUtils {
public:

    static bool hasSkill(u16 skill, const PlayerData &playerData) {
        for (auto s: playerData.skills) {
            if (s == skill) return true;
        }
        return false;
    }

    static bool hasSkill(const PriceListEntry &skill, const PlayerData &playerData) {
        return hasSkill(skill.skill, playerData);
    }

    static bool canHaveSkill(const PriceListEntry &skill, const PlayerData &playerData) {
        return playerData.magic >= skill.constraints.magic &&
               playerData.strength >= skill.constraints.strength &&
               playerData.dexterity >= skill.constraints.dexterity;
    }

    static void addSkill(u16 skill, PlayerData *playerData) {
        if (!hasSkill(skill, *playerData)) {
            playerData->skills[playerData->skills_count] = (portal_Skill) skill;
            playerData->skills_count++;
        }
    }

    static void removeSkill(u16 skill, PlayerData *playerData) {
        bool shifting = false;
        for (int i = 0; i < playerData->skills_count; ++i) {
            if (shifting) {
                playerData->skills[i - 1] = playerData->skills[i];
            }
            if (playerData->skills[i] == skill) {
                playerData->skills_count--;
                // last/first index
                if (i == playerData->skills_count - 1 || i == 0) {
                    playerData->skills[i] = portal_Skill_Skill_Default;
                }
                shifting = true;
            }
        }
    }

    static void removeSkill(const PriceListEntry &skill, PlayerData *playerData) {
        removeSkill(skill.skill, playerData);
    }
};


#endif //ESP32_PORTAL_FRAMEWORK_PLAYERDATAUTILS_H
