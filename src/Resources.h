#ifndef KIOSK_RESOURCES_H
#define KIOSK_RESOURCES_H

#include <Storage.h>
#include <types.h>
#include <map>
#include <vector>
#include <optional>

struct PriceListEntryConstraints {
    u8 strength;
    u8 magic;
    u8 dexterity;
};

struct PriceListEntry {
    String code;
    String altName;
    Operation operation;
    PriceListEntryConstraints constraints;
    u16 skill;
};

struct SkillsListEntry {
    std::string name;
    std::string description;
    PriceListEntryConstraints constraints;
    u16 skill;
};

struct PlayerMetadata {
    std::string name;
    u8 group;
};

class PriceList;

class SkillsList;

class PlayersMetadata;

class Resources {
public:
    explicit Resources(Storage *storage);

    bool begin();

    PlayersMetadata *loadPlayersMeta();

    PriceList *loadPriceList();

    SkillsList *loadSkillsList();

private:
    Storage *storage;
};

class PlayersMetadata {
public:
    explicit PlayersMetadata(std::map<u8, PlayerMetadata> players);

    PlayerMetadata getPlayerMetadata(u8 userId);

private:
    std::map<u8, PlayerMetadata> players;
};

class PriceList {
public:
    explicit PriceList(std::map<String, PriceListEntry> priceList);

    std::optional<PriceListEntry> getItemForCode(const String &code);

private:
    std::map<String, PriceListEntry> priceList;
};

typedef std::vector<SkillsListEntry>::const_iterator SkillsIterator;

class SkillsList {
public:
    explicit SkillsList(std::vector<SkillsListEntry> skillsList);

    SkillsIterator getSkillsPageStart(u8 pageNo, u8 pageSize);

    [[nodiscard]] u8 getLength() const { return skillsList.size(); }

private:
    std::vector<SkillsListEntry> skillsList;
};

#endif //KIOSK_RESOURCES_H
